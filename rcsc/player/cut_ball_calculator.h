//
// Created by nader on 4/27/24.
//

#ifndef LIBCYRUS_CUT_BALL_CALCULATOR_H
#define LIBCYRUS_CUT_BALL_CALCULATOR_H
#include <rcsc/geom/vector_2d.h>
#include <rcsc/geom/line_2d.h>
#include <rcsc/geom/circle_2d.h>
#include <rcsc/geom/rect_2d.h>
#include <rcsc/player/world_model.h>
#include <rcsc/player/abstract_player_object.h>

using namespace rcsc;
class CutBallCalculator {
public:
    int cycles_to_cut_ball_with_safe_thr_dist(const AbstractPlayerObject * player,
                                               const Vector2D & ball_pos,
                                               const int max_cycle,
                                               bool check_tackle,
                                               int & dash_cycle,
                                               int & turn_cycle,
                                               int & view_cycle,
                                               Vector2D predict_pos = Vector2D::INVALIDATED,
                                               Vector2D predict_vel = Vector2D::INVALIDATED,
                                               double safe_dist_thr = 0,
                                               double predict_body_double = -360.0,
                                               bool check_omni=false) const {

        //    	update_opps_pos(wm);
        if(!predict_pos.isValid())
            predict_pos = player->pos();
        if(!predict_vel.isValid())
            predict_vel = player->vel();
        AngleDeg predict_body = player->body();
        if (predict_body_double != -360.0)
            predict_body = AngleDeg(predict_body_double);
        int reach_cycle_kick = 1000;
        int reach_cycle_tackle = 1000;
        int reach_cycle_omni = 1000;

        int direct_kick_dash_cycle = 100;
        int direct_tackle_dash_cycle = 100;
        int omni_kick_dash_cycle = 100;

        int direct_kick_turn_cycle = 90;
        int direct_tackle_turn_cycle = 90;
        int omni_kick_turn_cycle = 90;

        int direct_kick_view_cycle = 0;
        int direct_tackle_view_cycle = 0;
        int omni_kick_view_cycle = 0;

        reach_cycle_kick = cycles_to_cut_ball_direct_kick(player,
                                                           ball_pos,
                                                           max_cycle,
                                                           direct_kick_dash_cycle,
                                                           direct_kick_turn_cycle,
                                                           direct_kick_view_cycle,
                                                           predict_pos,
                                                           predict_vel,
                                                           predict_body,
                                                           safe_dist_thr);
        if (check_tackle)
            reach_cycle_tackle = cycles_to_cut_ball_direct_tackle(player,
                                                                   ball_pos,
                                                                   max_cycle,
                                                                   direct_tackle_dash_cycle,
                                                                   direct_tackle_turn_cycle,
                                                                   direct_tackle_view_cycle,
                                                                   predict_pos,
                                                                   predict_vel,
                                                                   predict_body);
        if (max_cycle <= 5 && check_omni)
            reach_cycle_omni = cycles_to_cut_ball_omni_kick(player,
                                                             ball_pos,
                                                             max_cycle,
                                                             omni_kick_dash_cycle,
                                                             omni_kick_turn_cycle,
                                                             omni_kick_view_cycle,
                                                             predict_pos,
                                                             predict_vel,
                                                             predict_body,
                                                             safe_dist_thr);
        //        }
        //    		if(opp_type[opponent->unum()]!=-1){
        //    			reach_cycle_omni = predictOpponentReachStep_omni(wm,opponent,ball_pos,cycle,opp_min_dif_cycle);
        //    		}
        //		    	}

        //        dlog.addText(Logger::DRIBBLE,"ck:%d %d %d,ct:%d %d %d",reach_cycle_kick,direct_kick_dash_cycle,direct_kick_turn_cycle,reach_cycle_tackle,direct_tackle_dash_cycle,direct_tackle_turn_cycle);
        int opp_reach_cycle = reach_cycle_kick;
        dash_cycle = direct_kick_dash_cycle;
        turn_cycle = direct_kick_turn_cycle;
        view_cycle = direct_kick_view_cycle;
        if (reach_cycle_omni < opp_reach_cycle && check_omni){
            opp_reach_cycle = reach_cycle_omni;
            dash_cycle = omni_kick_dash_cycle;
            turn_cycle = omni_kick_turn_cycle;
            view_cycle = omni_kick_view_cycle;
        }
        if( reach_cycle_tackle < opp_reach_cycle && check_tackle){
            opp_reach_cycle = reach_cycle_tackle+1;
            dash_cycle = direct_tackle_dash_cycle+1;
            turn_cycle = direct_tackle_turn_cycle;
            view_cycle = direct_tackle_view_cycle;
        }
        return opp_reach_cycle;
    }
    int cycles_to_cut_ball(const AbstractPlayerObject * player, const Vector2D & ball_pos,
                            const int max_cycle,
                            bool check_tackle,
                            int & dash_cycle,
                            int & turn_cycle,
                            int & view_cycle,
                            Vector2D predict_pos = Vector2D::INVALIDATED,
                            Vector2D predict_vel = Vector2D::INVALIDATED,
                            double predict_body_double = -360.0,
                            bool check_omni=false) const {

        //    	update_opps_pos(wm);
        if(!predict_pos.isValid())
            predict_pos = player->pos();
        if(!predict_vel.isValid())
            predict_vel = player->vel();
        AngleDeg predict_body = player->body();
        if (predict_body_double != -360.0)
            predict_body = AngleDeg(predict_body_double);
        int reach_cycle_kick = 1000;
        int reach_cycle_tackle = 1000;
        int reach_cycle_omni = 1000;

        int direct_kick_dash_cycle = 100;
        int direct_tackle_dash_cycle = 100;
        int omni_kick_dash_cycle = 100;

        int direct_kick_turn_cycle = 90;
        int direct_tackle_turn_cycle = 90;
        int omni_kick_turn_cycle = 90;

        int direct_kick_view_cycle = 0;
        int direct_tackle_view_cycle = 0;
        int omni_kick_view_cycle = 0;

        reach_cycle_kick = cycles_to_cut_ball_direct_kick(player,
                                                           ball_pos,
                                                           max_cycle,
                                                           direct_kick_dash_cycle,
                                                           direct_kick_turn_cycle,
                                                           direct_kick_view_cycle,
                                                           predict_pos,
                                                           predict_vel,
                                                           predict_body,
                                                           0);
        if (check_tackle)
            reach_cycle_tackle = cycles_to_cut_ball_direct_tackle(player,
                                                                   ball_pos,
                                                                   max_cycle,
                                                                   direct_tackle_dash_cycle,
                                                                   direct_tackle_turn_cycle,
                                                                   direct_tackle_view_cycle,
                                                                   predict_pos,
                                                                   predict_vel,
                                                                   predict_body);
        //    		if(opp_type[opponent->unum()]!=-1){
        if (max_cycle <= 5 && check_omni)
            reach_cycle_omni = cycles_to_cut_ball_omni_kick(player,
                                                             ball_pos,
                                                             max_cycle,
                                                             omni_kick_dash_cycle,
                                                             omni_kick_turn_cycle,
                                                             omni_kick_view_cycle,
                                                             predict_pos,
                                                             predict_vel,
                                                             predict_body,
                                                             0);
        //        }
        //        dlog.addText(Logger::DRIBBLE,"ck:%d %d %d,ct:%d %d %d",reach_cycle_kick,direct_kick_dash_cycle,direct_kick_turn_cycle,reach_cycle_tackle,direct_tackle_dash_cycle,direct_tackle_turn_cycle);
        int opp_reach_cycle = reach_cycle_kick;
        dash_cycle = direct_kick_dash_cycle;
        turn_cycle = direct_kick_turn_cycle;
        view_cycle = direct_kick_view_cycle;
        if (reach_cycle_omni < opp_reach_cycle && check_omni){
            opp_reach_cycle = reach_cycle_omni;
            dash_cycle = omni_kick_dash_cycle;
            turn_cycle = omni_kick_turn_cycle;
            view_cycle = omni_kick_view_cycle;
        }
        if( reach_cycle_tackle < opp_reach_cycle && check_tackle){
            opp_reach_cycle = reach_cycle_tackle+1;
            dash_cycle = direct_tackle_dash_cycle+1;
            turn_cycle = direct_tackle_turn_cycle;
            view_cycle = direct_tackle_view_cycle;
        }
        return opp_reach_cycle;
    }
    int cycles_to_cut_ball_direct_kick(const AbstractPlayerObject * player,
                                        const Vector2D & ball_pos,
                                        const int cycle,
                                        int & dash_cycle,
                                        int & turn_cycle,
                                        int & view_cycle,
                                        Vector2D predict_pos,
                                        Vector2D predict_vel,
                                        AngleDeg predict_body,
                                        double safe_dist_thr) const {
        view_cycle = 0;
        static const Rect2D penalty_area(
            Vector2D(ServerParam::i().theirPenaltyAreaLineX(),
                      -ServerParam::i().penaltyAreaHalfWidth()),
            Size2D(ServerParam::i().penaltyAreaLength(),
                    ServerParam::i().penaltyAreaWidth()));
        static const double CONTROL_AREA_BUF = 0.1;
        const ServerParam & SP = ServerParam::i();

        const PlayerType * ptype = player->playerTypePtr();

        double control_area = (
            player->goalie() && penalty_area.contains(ball_pos) ?
                                                          SP.catchableArea() : ptype->kickableArea());
        Vector2D opp_pos = predict_pos;
        Vector2D iner_pos = ptype->inertiaPoint(opp_pos,predict_vel,cycle);

        double target_dist = opp_pos.dist(ball_pos);
        double dash_dist = iner_pos.dist(ball_pos) - safe_dist_thr;

        bool back_dash = false;
        double next_body_angle = predict_body.degree();
        int n_turn = predict_player_turn_cycle_direct_kick(ptype,
                                                            predict_body,
                                                            predict_vel.r(),
                                                            target_dist,
                                                            (ball_pos - opp_pos).th(),
                                                            control_area,
                                                            predict_pos,
                                                            predict_vel,
                                                            ball_pos,
                                                            back_dash,
                                                            next_body_angle);
        if (target_dist - control_area
             > ptype->realSpeedMax() * (double) (cycle + player->posCount())) {
            turn_cycle = n_turn;
            dash_cycle = ptype->cyclesToReachDistance(target_dist);
            //            dlog.addText(Logger::PASS, "ctcb: return A");
            return std::max(cycle + 1, dash_cycle + turn_cycle + 1);
        }
        if (target_dist - control_area < 0.001) {
            dash_cycle = 0;
            turn_cycle = 0;
            return 0;
        }

        int reach_step_with_out_action = 1000;
        for (int c = 1; c <= cycle; c++) {
            Vector2D c_inertia_pos = ptype->inertiaPoint(predict_pos, predict_vel, c);
            double dash_dist = c_inertia_pos.dist(ball_pos);
            if (dash_dist - control_area - CONTROL_AREA_BUF < 0.001) {
                reach_step_with_out_action = c;
                //                dlog.addText(Logger::PASS, "ctcb: wotjout %d", reach_step_with_out_action);
                break;
            }
        }

        Line2D player_move_line = Line2D(opp_pos, next_body_angle);
        Circle2D intercept_circle = Circle2D(ball_pos, control_area);
        Vector2D intercept_1 = Vector2D::INVALIDATED;
        Vector2D intercept_2 = Vector2D::INVALIDATED;
        Vector2D best_intercept;
        int intercept_number = intercept_circle.intersection(player_move_line,
                                                              &intercept_1, &intercept_2);
        if (intercept_number == 0) {
            best_intercept = ball_pos;
        } else if (intercept_number == 1) {
            best_intercept = intercept_1;
        } else {
            if (intercept_1.dist(predict_pos) < intercept_2.dist(predict_pos)) {
                best_intercept = intercept_1;
            } else {
                best_intercept = intercept_2;
            }
        }
        //	cout<<"   predictOpponentReachStep_direct_kick5"<<endl;
        double cycle_to_reach_angle = 0.0;
        int add_dash = 0;
        if (intercept_number == 0){
            Vector2D last_pos = player_move_line.intersection(Line2D(ball_pos, next_body_angle + 90.0));
            if (last_pos.dist(iner_pos) > control_area){
                dash_dist = last_pos.dist(iner_pos);
            } else{
                // if (!is_tm){
                if (ball_pos.dist(iner_pos) < 0.15){
                    dash_dist = 0.0;
                    add_dash += 1;
                }else{
                    dash_dist = ball_pos.dist(iner_pos) - control_area;
                    cycle_to_reach_angle = (ball_pos - iner_pos).th().degree();
                }
                //}else{
                //  dash_dist = ball_pos.dist(iner_pos) - control_area;
                //cycle_to_reach_angle = (ball_pos - iner_pos).th().degree();
                //}
            }

        }else{
            dash_dist = iner_pos.dist(best_intercept);
        }
        dash_dist -= safe_dist_thr;
        int n_dash = 0;
        if (dash_dist > 0){
            n_dash = ptype->cyclesToReachDistanceOnDashDir(dash_dist, cycle_to_reach_angle);
        }
        n_dash += add_dash;
        if (n_turn > 0) {
            view_cycle = 1;
        }
        if (player->vel().r() < 0.15)
            view_cycle = 1;
        else {		//63540798
            Vector2D opp_vel = predict_vel;
            opp_vel.rotate((opp_pos - ball_pos).th());
            if (opp_vel.x < 0.1)
                view_cycle = 1;
        }
        if (n_dash == 0)
            view_cycle = 0;
        //        dlog.addText(Logger::PASS, "bestinter %.1f,%.1f, pp %.1f %.f, pv %.1f,%.1f, ip %.1f, %.1f, dd %.1f, ndash %d, vc %d, nt %d, nd %d"
        //                     , best_intercept.x, best_intercept.y
        //                     ,opp_pos.x, opp_pos.y
        //                     ,predict_vel.x, predict_vel.y
        //                     ,iner_pos.x, iner_pos.y
        //                     , dash_dist, n_dash, view_cycle, n_turn, n_dash);
        int n_step = n_turn + n_dash + view_cycle;

        if(n_step < reach_step_with_out_action){
            dash_cycle = n_dash;
            turn_cycle = n_turn;
            return n_step;
        }else{
            dash_cycle = reach_step_with_out_action;
            turn_cycle = 0;
            view_cycle = 0;
            return reach_step_with_out_action;
        }
    }
    int predict_player_turn_cycle_direct_kick(const rcsc::PlayerType * ptype,
                                               const rcsc::AngleDeg & player_body,
                                               const double & player_speed,
                                               const double & target_dist,
                                               const rcsc::AngleDeg & target_angle,
                                               const double & dist_thr,
                                               const rcsc::Vector2D player_pos,
                                               const rcsc::Vector2D player_vel,
                                               const rcsc::Vector2D target_pos,
                                               bool & use_back_dash,
                                               double & next_body_angle) const {
        const ServerParam & SP = ServerParam::i();

        int n_turn = 0;

        double angle_diff = (target_angle - player_body).abs();
        //        dlog.addText(Logger::DRIBBLE,"ta:%.1f,pb:%.1f,ad:%.1f",target_angle.degree(),player_body.degree(),angle_diff);
        if (angle_diff > 180){
            angle_diff = 360 - angle_diff;
        }
        if (use_back_dash && target_dist < 5.0 // Magic Number
             && angle_diff > 90.0 && SP.minDashPower() < -SP.maxDashPower() + 1.0) {
            angle_diff = std::fabs(angle_diff - 180.0);  // assume backward dash
            if(angle_diff > 180)
                angle_diff = 360 - angle_diff;
            use_back_dash = true;
        } else{
            if(angle_diff > 180)
                angle_diff = 360 - angle_diff;
            use_back_dash = false;
        }
        //        dlog.addText(Logger::DRIBBLE,"ta:%.1f,pb:%.1f,ad:%.1f",target_angle.degree(),player_body.degree(),angle_diff);
        double speed = player_speed;
        Vector2D next_pos = player_pos;
        Vector2D next_vel = player_vel;
        //        dlog.addText(Logger::DRIBBLE,"%.1f,%.1f",angle_diff,Line2D(next_pos, next_body_angle).dist(target_pos));
        while (Line2D(next_pos, next_body_angle).dist(target_pos) > dist_thr + 0.15) {
            //            dlog.addText(Logger::DRIBBLE,"%.1f,%.1f",angle_diff,Line2D(next_pos, next_body_angle).dist(target_pos));

            next_pos += next_vel;
            next_vel *= ptype->playerDecay();
            angle_diff -= ptype->effectiveTurn(SP.maxMoment(), speed);
            speed *= ptype->playerDecay();
            ++n_turn;
            if (angle_diff <= 0) {
                next_body_angle = target_angle.degree();
                break;
            } else {
                next_body_angle = target_angle.degree() + angle_diff;
            }
        }

        return n_turn;
    }
    int cycles_to_cut_ball_direct_tackle(const AbstractPlayerObject * player,
                                          const Vector2D & ball_pos,
                                          const int cycle,
                                          int & dash_cycle,
                                          int & turn_cycle,
                                          int & view_cycle,
                                          Vector2D predict_pos,
                                          Vector2D predict_vel,
                                          AngleDeg predict_body) const {
        view_cycle = 0;
        static const Rect2D penalty_area(
            Vector2D(ServerParam::i().theirPenaltyAreaLineX(),
                      -ServerParam::i().penaltyAreaHalfWidth()),
            Size2D(ServerParam::i().penaltyAreaLength(),
                    ServerParam::i().penaltyAreaWidth()));

        const ServerParam & SP = ServerParam::i();

        const PlayerType * ptype = player->playerTypePtr();

        double control_area = (
            player->goalie() && penalty_area.contains(ball_pos) ?
                                                          SP.catchableArea() : ptype->kickableArea());
        Vector2D opp_pos = predict_pos;
        const Vector2D inertia_pos = ptype->inertiaPoint(opp_pos,predict_vel,cycle);
        const double target_dist = inertia_pos.dist(ball_pos);

        if (target_dist
             > ptype->realSpeedMax() * (double) (cycle + player->posCount())
                   + SP.tackleDist()) {
            //            dlog.addText(Logger::DRIBBLE,"pos(%.2f,%.2f),tardist:%.2f, high dist",opp_pos.x,opp_pos.y,target_dist);
            turn_cycle = 3;
            dash_cycle = ptype->cyclesToReachDistance(target_dist) + 2;
            return dash_cycle + turn_cycle;
        }

        if (target_dist - control_area < 0.001) {
            //            dlog.addText(Logger::DRIBBLE,"opppos(%.2f,%.2f),tardist:%.2f, low dist",opp_pos.x,opp_pos.y,target_dist);
            dash_cycle = 0;
            turn_cycle = 0;
            return 0;
        }

        double dash_dist = target_dist;

        bool back_dash = false;

        bool should_turn = true;
        Line2D opp_body_line = Line2D(inertia_pos,predict_body);
        if(opp_body_line.dist(ball_pos) < 1){
            double dif = ((ball_pos-inertia_pos).th() - AngleDeg(predict_body)).abs();
            if(dif > 180)
                dif = 360 - dif;
            if(dif < 90)
                should_turn = false;
        }
        double opp_ball_angle = (ball_pos - inertia_pos).th().degree();
        Vector2D tmp1 = inertia_pos
                      + Vector2D::polar2vector(ptype->kickableArea(),
                                                opp_ball_angle + 90);
        Vector2D tmp2 = inertia_pos
                      + Vector2D::polar2vector(ptype->kickableArea(),
                                                opp_ball_angle - 90);
        double tmp1_angle = (ball_pos - tmp1).th().degree();
        double tmp1_angle_dif = tmp1_angle - player->body().degree();
        if (tmp1_angle_dif < 0)
            tmp1_angle_dif *= (-1.0);
        if (tmp1_angle_dif > 180)
            tmp1_angle_dif = 360 - tmp1_angle_dif;
        double tmp2_angle = (ball_pos - tmp2).th().degree();
        double tmp2_angle_dif = tmp2_angle - player->body().degree();
        if (tmp2_angle_dif < 0)
            tmp2_angle_dif *= (-1.0);
        if (tmp2_angle_dif > 180)
            tmp2_angle_dif = 360 - tmp2_angle_dif;

        double body_target;
        if (tmp1_angle_dif < tmp2_angle_dif)
            body_target = tmp1_angle;
        else
            body_target = tmp2_angle;
        double next_body_angle = player->body().degree();
        int n_turn = (should_turn?predict_player_turn_cycle_direct_tackle(ptype, predict_body,
                                                                              predict_vel.r(), target_dist, body_target, control_area, opp_pos,
                                                                              predict_vel, ball_pos, back_dash, next_body_angle):0);

        //        dlog.addText(Logger::DRIBBLE,"opppos(%.2f,%.2f),tardist:%.2f, targangle:%.2f, nturn:%d",opp_pos.x,opp_pos.y,target_dist,body_target,n_turn);
        Line2D player_move_line = Line2D(inertia_pos, next_body_angle);
        Vector2D ball_on_line = player_move_line.projection(ball_pos);
        dash_dist = inertia_pos.dist(ball_on_line)
                  - ServerParam::i().tackleDist();

        int n_dash = ptype->cyclesToReachDistance(dash_dist);
        //        dlog.addText(Logger::DRIBBLE,"opppos(%.2f,%.2f),tardist:%.2f, targangle:%.2f, nturn:%d, dash_dist:%.2f,ndash:%d",opp_pos.x,opp_pos.y,target_dist,body_target,n_turn,dash_dist,n_dash);
        view_cycle = 0;
        if (n_turn > 0) {
            view_cycle = 1;
        }
        if (player->vel().r() < 0.15)
            view_cycle = 1;
        else {    //63540798
            Vector2D opp_vel = player->vel();
            opp_vel.rotate((inertia_pos - ball_pos).th());
            if (opp_vel.x < 0.1)
                view_cycle = 1;
        }
        int n_step = n_turn + n_dash + view_cycle;

        dash_cycle = n_dash;
        turn_cycle = n_turn;
        return n_step;
    }

    int predict_player_turn_cycle_direct_tackle(const rcsc::PlayerType * ptype,
                                                 const rcsc::AngleDeg & player_body,
                                                 const double & player_speed,
                                                 const double & /*target_dist*/,
                                                 const rcsc::AngleDeg & target_angle,
                                                 const double & dist_thr,
                                                 const rcsc::Vector2D player_pos,
                                                 const rcsc::Vector2D player_vel,
                                                 const rcsc::Vector2D target_pos,
                                                 bool & /*use_back_dash*/,
                                                 double & next_body_angle) const {
        const ServerParam & SP = ServerParam::i();

        int n_turn = 0;

        double angle_diff = (target_angle - player_body).abs();
        //        dlog.addText(Logger::DRIBBLE,"ta:%.1f,pb:%.1f,ad:%.1f",target_angle.degree(),player_body.degree(),angle_diff);
        if (angle_diff > 180){
            angle_diff = 360 - angle_diff;
        }

        double speed = player_speed;
        Vector2D next_pos = player_pos;
        Vector2D next_vel = player_vel;
        //        dlog.addText(Logger::DRIBBLE,"%.1f,%.1f",angle_diff,Line2D(next_pos, next_body_angle).dist(target_pos));
        while (Line2D(next_pos, next_body_angle).dist(target_pos) > dist_thr) {
            //            dlog.addText(Logger::DRIBBLE,"%.1f,%.1f",angle_diff,Line2D(next_pos, next_body_angle).dist(target_pos));

            next_pos += next_vel;
            next_vel *= ptype->playerDecay();
            angle_diff -= ptype->effectiveTurn(SP.maxMoment(), speed);
            speed *= ptype->playerDecay();
            ++n_turn;
            if (angle_diff <= 0) {
                next_body_angle = target_angle.degree();
                break;
            } else {
                next_body_angle = target_angle.degree() + angle_diff;
            }
        }

        return n_turn;

    }
    int cycles_to_cut_ball_omni_kick(const AbstractPlayerObject * player,
                                      const Vector2D & ball_pos,
                                      const int cycle,
                                      int & dash_cycle,
                                      int & turn_cycle,
                                      int & view_cycle,
                                      Vector2D predict_pos,
                                      Vector2D predict_vel,
                                      AngleDeg predict_body,
                                      double /*safe_dist_thr*/) const{
        static const Rect2D penalty_area(
            Vector2D(ServerParam::i().theirPenaltyAreaLineX(),
                      -ServerParam::i().penaltyAreaHalfWidth()),
            Size2D(ServerParam::i().penaltyAreaLength(),
                    ServerParam::i().penaltyAreaWidth()));

        const ServerParam & SP = ServerParam::i();

        const PlayerType * ptype = player->playerTypePtr();

        double control_area = ptype->kickableArea();

        int final_step = 1000;
        for(double dir = -180; dir < 180; dir += 30.0){
            double dash_dir_deg = AngleDeg(dir).abs();
            dash_dir_deg /= 10.0;
            int dash_dir_step = static_cast<int>(std::round(dash_dir_deg));
            AngleDeg dash_angle = predict_body + AngleDeg(dir);
            int max_check_cycle = 2;
            double max_move_dist = ptype->dashDistanceTableOnDashDir()[dash_dir_step][max_check_cycle - 1] + predict_vel.rotate(-dir).x;
            if (max_move_dist > ball_pos.dist(predict_pos) - control_area - 0.1)
                continue;
            for (int c = 1; c <= max_check_cycle && c <= cycle; c++){
                double move = ptype->dashDistanceTableOnDashDir()[dash_dir_step][c - 1];
                Vector2D opp_pos = predict_pos;
                Vector2D inertia_pos = ptype->inertiaPoint(opp_pos,predict_vel,c);
                inertia_pos += (Vector2D::polar2vector(move, dash_angle));
                control_area = ptype->kickableArea();
                if (player->goalie() && penalty_area.contains(ball_pos))
                    if (((ball_pos - inertia_pos).th() - predict_body).abs() < 90.0)
                        control_area = SP.catchableArea();
                if(inertia_pos.dist(ball_pos) < control_area){
                    int n_step = 1 + 0 + c;
                    if (c == 1){
                        view_cycle = 1;
                        turn_cycle = 0;
                        dash_cycle = c;
                        return n_step;
                    }
                    if (n_step < final_step){
                        view_cycle = 1;
                        turn_cycle = 0;
                        dash_cycle = c;
                        final_step = n_step;
                    }
                }
            }
        }
        return final_step;
    }
    //    int cycles_to_cut_ball_omni_kick(const WorldModel & wm,
    //                                       const Vector2D & ball_pos,
    //                                       const int cycle,
    //                                       int & dash_cycle,
    //                                       int & turn_cycle,
    //                                       int & view_cycle,
    //                                       Vector2D predict_pos,
    //                                       Vector2D predict_vel,
    //                                       AngleDeg predict_body,
    //                                       double safe_dist_thr) const{
    //        static const Rect2D penalty_area(
    //                Vector2D(ServerParam::i().theirPenaltyAreaLineX(),
    //                         -ServerParam::i().penaltyAreaHalfWidth()),
    //                Size2D(ServerParam::i().penaltyAreaLength(),
    //                       ServerParam::i().penaltyAreaWidth()));
    //
    //        const ServerParam & SP = ServerParam::i();
    //
    //        const PlayerType * ptype = playerTypePtr();
    //
    //        double control_area = (
    //                goalie() && penalty_area.contains(ball_pos) ?
    //                SP.catchableArea() : ptype->kickableArea());
    //
    //        for(double dir = -180; dir < 180; dir+=45){
    //            AngleDeg dash_angle = predict_body + AngleDeg(dir);
    //            double dash_rate = ptype->dashPowerRate() * SP.dashDirRate( dir );
    //            Vector2D accel = Vector2D::polar2vector(100 * dash_rate, dash_angle);
    //            for (int c = 1; c <= 2 && c < cycle; c++){
    //                Vector2D opp_pos = predict_pos;
    //                Vector2D inertia_pos = ptype->inertiaPoint(opp_pos,predict_vel,c);
    //                inertia_pos += (accel*c);
    //                if(inertia_pos.dist(ball_pos) < control_area){
    //                    view_cycle = 1;
    //                    turn_cycle = 0;
    //                    dash_cycle = c;
    //                    return turn_cycle + dash_cycle + view_cycle;
    //                }
    //            }
    //        }
    //        return 1000;
    //    }
};
#endif //LIBCYRUS_CUT_BALL_CALCULATOR_H
