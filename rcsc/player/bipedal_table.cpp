#include <rcsc/player/bipedal_table.h>
#include <rcsc/common/player_param.h>
#include <rcsc/common/server_param.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace rcsc;

// #define DB(msg) std::cout << msg << std::endl;
#define DB(msg) ;

double round_to_precision(double num, double precision) {
    double factor = 1.0 / precision;
    return round(num * factor) / factor;
}

double
check_and_normalize_dash_dir( const WorldModel & wm,
                                double dir )

{
    const ServerParam & param = ServerParam::i();

    if ( dir < param.minDashAngle() - 0.001
            || param.maxDashAngle() + 0.001 < dir )
    {
        dlog.addText( Logger::ACTION,
                        __FILE__" (setDash) exceeding the dash angle range %.1f", dir );
        std::cerr << wm.teamName() << ' ' << wm.self().unum() << ": " << wm.time()
                    << " exceeding the dash angle range: " << dir
                    << std::endl;
        dir = param.normalizeDashAngle( dir );
    }

    return param.discretizeDashAngle( dir );
}

AStarState* find_min_f(std::vector<AStarState>& open_list) {
    AStarState* min_state = nullptr;
    double min_f = 1000000.0;
    for (AStarState& state : open_list) {
        if (state.f < min_f) {
            min_f = state.f;
            min_state = &state;
        }
    }
    return min_state;
}

AStarState* simulate_next_state(const AStarState* state,
                                const AStarAction& action,
                                const WorldModel& wm,
                                const double & dash_rate,
                                const double & player_decay) {
    DB("NSA")
    double left_command_dir = check_and_normalize_dash_dir(wm, action.dir_l.degree());
    double right_command_dir = check_and_normalize_dash_dir(wm, action.dir_r.degree());

    double left_dir_rate = ServerParam::i().dashDirRate(left_command_dir);
    double right_dir_rate = ServerParam::i().dashDirRate(right_command_dir);

    double left_accel_mag = std::fabs(action.power_l * left_dir_rate * dash_rate); // TODO Not current dash rate?
    double right_accel_mag = std::fabs(action.power_r * right_dir_rate * dash_rate);

    DB("NSB")
    AngleDeg left_accel_angle = state->M_body + left_command_dir;
    AngleDeg right_accel_angle = state->M_body + right_command_dir;

    if (action.power_l < 0.0) left_accel_angle += 180.0;
    if (action.power_r < 0.0) right_accel_angle += 180.0;

    DB("NSC")
    const Vector2D left_accel = Vector2D::from_polar(left_accel_mag, left_accel_angle);
    const Vector2D right_accel = Vector2D::from_polar(right_accel_mag, right_accel_angle);

    const Vector2D body_unit = Vector2D::from_polar(1.0, state->M_body);
    const Vector2D vel_l = state->M_vel + left_accel;
    const Vector2D vel_r = state->M_vel + right_accel;

    DB("NSD")
    const double vel_l_body = body_unit.x * vel_l.x + body_unit.y * vel_l.y;
    const double vel_r_body = body_unit.x * vel_r.x + body_unit.y * vel_r.y;

    const double omega = (vel_l_body - vel_r_body) / (ServerParam::i().defaultPlayerSize() * 2.0);
    const Vector2D new_vel = (vel_r + vel_l) * 0.5;

    DB("NSE")
    double dash_power = (action.power_l < 0.0 ? -action.power_l : action.power_l * 0.5) + (action.power_r < 0.0 ? -action.power_r : action.power_r * 0.5);
    double left_dash_power = action.power_l;
    double right_dash_power = action.power_r;

    double dash_rotation = AngleDeg::rad2deg(omega);
    Vector2D dash_accel = new_vel - state->M_vel;

    const double actual_accel_mag = dash_accel.r();
    if (actual_accel_mag > ServerParam::i().playerAccelMax()) {
        dash_accel *= ServerParam::i().playerAccelMax() / actual_accel_mag;
    }
    DB("NSF")

    AStarState *next_state = new AStarState();
    next_state->M_pos = state->M_pos + new_vel;
    next_state->M_vel = new_vel * player_decay;
    next_state->M_body = state->M_body + dash_rotation;
    next_state->action = action;
    next_state->parent = state;

    return next_state;
}

std::vector<AStarAction>
get_actions(const AStarState* state, const PlayerType& ptype) {
    std::vector<AStarAction> actions;
    const double POWER_THR = 25;
    const double DIR_THR = 22.5;
    const ServerParam & SP = ServerParam::i();

    for (double power_l = 25; power_l <= SP.maxDashPower(); power_l += POWER_THR) { // 2
        for (double power_r = 25; power_r <= SP.maxDashPower(); power_r += POWER_THR) { //2
            for (double dir_l = -157.5; dir_l <= 180; dir_l += DIR_THR) { //
                for (double dir_r = -157.5; dir_r <= 180; dir_r += DIR_THR) {
                    AStarAction action;
                    action.power_l = power_l;
                    action.power_r = power_r;
                    action.dir_l = AngleDeg(dir_l);
                    action.dir_r = AngleDeg(dir_r);
                    actions.push_back(action);
                }
            }
        }
    }

    return actions;
}


AStarAction
TargetActionTable::a_star(const WorldModel& wm,
                          const Vector2D& target,
                          const double & dash_rate,
                          const double & player_decay,
                          std::vector<Vector2D> &trajectory) const
{
    const double THR = 0.01;
    const ServerParam & SP = ServerParam::i();
    const PlayerType & ptype = wm.self().playerType();

    const Vector2D start_pos = Vector2D(0, 0);
    Vector2D self_pos = Vector2D(0, 0);
    Vector2D self_vel = Vector2D(0, 0);
    AngleDeg self_body = AngleDeg(0);
    const Vector2D target_rel = target - self_pos;

    std::vector< AStarState* > open_list;
    std::vector< AStarState* > close_list;

    auto init_state = new AStarState(self_pos);
    init_state->g = 0;
    init_state->h = target_rel.r();
    init_state->f = init_state->g + init_state->h;
    init_state->M_body = self_body;
    init_state->M_vel = self_vel;
    init_state->parent = nullptr;
    open_list.push_back(init_state);

    AStarState* end_state = nullptr;
    int n = 64*32;
    while (!open_list.empty()) {
        DB("While loop IT")
        AStarState* q = open_list[0];
        open_list.erase(open_list.begin());
        std::vector<AStarAction> actions = get_actions(q, ptype);
        int i = 0;
        for (const AStarAction& action : actions) {
            DB("Action loop IT")
            // std::cout << "Action: " << i++  << "|" <<  n << std::endl;
            AStarState* new_state = simulate_next_state(q, action, wm, dash_rate, player_decay);
            dlog.addCircle(Logger::ACTION, new_state->M_pos, 0.2, "#ff0000", true);
            DB("NS")
            if (new_state->M_pos.dist(target_rel) < 1.) {
                end_state = new_state;
                close_list.push_back(new_state);
                DB("BREAKING B")
                break;
            }
            DB("IF")

            new_state->g = q->g + q->M_pos.dist(new_state->M_pos) / 10.;
            new_state->h = new_state->M_pos.dist(target) / 10.;
            new_state->h += (new_state->M_body - target_rel.th()).abs() /360.;
            new_state->f = -new_state->g + new_state->h;
            DB("EV")

            // if (is_in_list(new_state, open_list, THR) || is_in_list(new_state, close_list, THR)) {
            //     DB("CONTINUE C")
            //     continue;
            // }
            open_list.push_back(new_state);
            DB("E")
        }
        std::sort(open_list.begin(), open_list.end(), [](const AStarState* a, const AStarState* b) {
            return a->f < b->f;
        });
        close_list.push_back(q);
        if (end_state != nullptr) {
            DB("BREAKING D")
            break;
        }
    }

    const AStarState* state = end_state;
    AStarAction action = state->action;
    while (state->parent != nullptr) {
        trajectory.push_back(state->M_pos);
        action = state->action;
        state = state->parent;
    }

    // delete all states
    for (AStarState* s : open_list) {
        delete s;
    }
    for (AStarState* s : close_list) {
        delete s;
    }

    //reverse trajectory
    std::reverse(trajectory.begin(), trajectory.end());

    return action;
}

void
TargetActionTable::initial(const WorldModel& wm, bool gen) {
    if (M_initialized) return;
    if (gen){
        if (wm.self().unum() == 1) return;
        const ServerParam & SP = ServerParam::i();
        const PlayerType & ptype = wm.self().playerType();

        const double dist = 10;
        int n = 0;
        const int u = wm.self().unum();
        for (double player_decay = 0.4-0.11; player_decay <= 0.4 + 0.11; player_decay += 0.005) {
            for (double dash_rate = 0.006 - 0.0013; dash_rate < 0.006 + 0.0009; dash_rate += 0.00005) {
                if (n++ % 10 != u - 2) continue; 
                // open a file with name PT_PlayerDecay_DashRate to write in it
                std::ofstream file;
                std::ostringstream player_decay_stream;
                player_decay_stream << std::fixed << std::setprecision(5) << player_decay;
                std::string player_decay_str = player_decay_stream.str();

                // Create an output string stream for dash_rate
                std::ostringstream dash_rate_stream;
                dash_rate_stream << std::fixed << std::setprecision(5) << dash_rate;
                std::string dash_rate_str = dash_rate_stream.str();

                // Concatenate to form the filename
                std::string filename = "PT_data/PT_" + player_decay_str + "_" + dash_rate_str + ".csv";
                file.open(filename);
                for (int i = 0; i < 360; i+=5){
                    std::cout << "INIT:\t" << player_decay << ",\t" << dash_rate << ",\t" << i << std::endl;
                    double angle = -180. + i;
                    Vector2D target = Vector2D::from_polar(dist, angle);
                    std::vector<Vector2D> trajectory;
                    AStarAction action = a_star(wm, target, dash_rate, player_decay, trajectory);
                    file << action.power_l << "," << action.dir_l.degree() << "," << action.power_r << "," << action.dir_r.degree();
                    for (Vector2D pos : trajectory) {
                        file << "," << pos.x << "," << pos.y;
                    }
                    file << std::endl;
                }
                file.close();
            }
        }
        M_initialized = true;
    } 
    else {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "My UNUM: " << wm.self().unum() << std::endl;
        std::cout << "PT data is being loaded" << std::endl;
        const ServerParam & SP = ServerParam::i();
        const PlayerParam & PP = PlayerParam::i();

        M_data = std::vector<std::vector<TargetActionPair>>();
        for (int id = 0; id < PP.playerTypes(); id++){
            std::cout << "PTYPE ID: " << id;
            const PlayerType * ptype = PlayerTypeSet::i().get(id);
            const double player_decay = round_to_precision(ptype->playerDecay(), 0.005);
            const double dash_rate = round_to_precision(ptype->dashPowerRate(), 0.00005);

            // make file_name
            std::ostringstream player_decay_stream;
            player_decay_stream << std::fixed << std::setprecision(3) << player_decay;
            std::string player_decay_str = player_decay_stream.str() + "00";

            std::ostringstream dash_rate_stream;
            dash_rate_stream << std::fixed << std::setprecision(5) << dash_rate;
            std::string dash_rate_str = dash_rate_stream.str();

            std::string filename = "PT_data/PT_" + player_decay_str + "_" + dash_rate_str + ".csv";
            std::cout << " Reading File: " << filename;
            std::cout << " With PlayerDecay: " << player_decay << " DashRate: " << dash_rate << std::endl;
            std::ifstream file;
            file.open(filename);

            M_data.push_back(std::vector<TargetActionPair>());
            for (int i = 0; i < 360; i+=5){
                TargetActionPair pair;
                pair.M_target = Vector2D::from_polar(10, -180. + i);
                
                double power_l, power_r, dir_l, dir_r;
                char ignore;
                file >> power_l >> ignore >> dir_l >> ignore >> power_r >> ignore >> dir_r;
                std::cout << "id: " << id << " PowerL: " << power_l << " DirL: " << dir_l << " PowerR: " << power_r << " DirR: " << dir_r << std::endl;
                pair.M_action = {power_l, AngleDeg(dir_l), power_r, AngleDeg(dir_r)};
                
                pair.M_trajectory = std::vector<Vector2D>();
                while (file.peek() != '\n'){
                    double x, y;
                    char ignore;
                    file >> ignore >> x >> ignore >> y;
                    pair.M_trajectory.push_back(Vector2D(x, y));
                }

                M_data[id].push_back(pair);
            }
        }
        std::cout << "PT data loaded" << std::endl;
        M_initialized = true;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Time taken to load PT data: " << elapsed.count() << " ms" << std::endl;
    }
}

AStarAction
TargetActionTable::get_action(const Vector2D &target,
                              const Vector2D& pos,
                              const AngleDeg &body,
                              const int& ptype_id) const {
    double min_dist = 1000000.0;
    Vector2D best_pos = Vector2D::INVALIDATED;
    Vector2D target_rel = target - pos;
    Vector2D rotated_target = target_rel.rotatedVector(-body);

    const int angle_index = round(rotated_target.th().degree() / 5.);

    dlog.addCircle(Logger::ACTION, target, 0.2, "#ff0000", true);
    dlog.addCircle(Logger::ACTION, rotated_target, 0.2, "#ff0000", true);
    dlog.addLine(Logger::ACTION, pos, target, "#000000");
    dlog.addLine(Logger::ACTION, Vector2D(0, 0), rotated_target, "#000000");


    AStarAction best_action = M_data[ptype_id][angle_index].M_action;

    dlog.addCircle(Logger::ACTION, best_pos, 0.2, "#0000ff", true);
    dlog.addText(Logger::ACTION, "Best Action: %f %f %f %f", best_action.power_l, best_action.dir_l.degree(), best_action.power_r, best_action.dir_r.degree());

    return best_action;
}

std::vector<Vector2D>
TargetActionTable::get_trajectory(const rcsc::Vector2D &target,
                                  const rcsc::Vector2D& pos,
                                  const rcsc::AngleDeg &body,
                                  const int& ptype_id) const {
    Vector2D target_rel = target - pos;
    target_rel = target_rel.rotatedVector(-body);

    const int angle_index = std::min((int)round((target_rel.th().degree() + 180.)/ 5.), 71);
    std::cout << "target_rel: " << target_rel.th().degree() << std::endl;
    std::cout << "angle_index: " << angle_index << std::endl;
    return M_data[ptype_id][angle_index].M_trajectory;
}