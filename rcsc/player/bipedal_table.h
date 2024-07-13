#include <rcsc/geom.h>
#include <rcsc/player/world_model.h>

class AStarAction
{
public:
    double power_l;
    rcsc::AngleDeg dir_l;
    double power_r;
    rcsc::AngleDeg dir_r;
};

class AStarState
{
public:
    rcsc::Vector2D M_pos;
    rcsc::Vector2D M_vel;
    rcsc::AngleDeg M_body;
    double g, h, f;
    const AStarState* parent = nullptr;
    AStarAction action;

    AStarState() {
        M_pos = rcsc::Vector2D::INVALIDATED;
        g = h = f = 1000;
        parent = nullptr;
    }

    AStarState( const rcsc::Vector2D & pos ) {
        M_pos = pos;
        g = h = f = 1000;
        parent = nullptr;
    }

    void evaluate(const rcsc::Vector2D & target){
    }

    bool operator==(const AStarState &rhs) const {
        return M_pos.dist(rhs.M_pos) < 0.1;
    }
};

class TargetActionPair
{
public:
    rcsc::Vector2D M_target;
    AStarAction M_action;
    std::vector<rcsc::Vector2D> M_trajectory;
};

class TargetActionTable
{
    public:
    std::vector<std::vector<TargetActionPair>> M_data;
    bool M_initialized = false;

    static TargetActionTable* instance() {
        static TargetActionTable *instance = new TargetActionTable();
        return instance;
    }

    AStarAction a_star(const rcsc::WorldModel &wm,
                       const rcsc::Vector2D &target,
                       const double & dash_rate,
                       const double & player_decay,
                       std::vector<rcsc::Vector2D> &trajectory) const;

    void initial(const rcsc::WorldModel &wm, const bool gen = false);
    AStarAction get_action(const rcsc::Vector2D &target,
                           const rcsc::Vector2D& pos,
                           const rcsc::AngleDeg &body,
                           const int& ptype_id) const;
    
    std::vector<rcsc::Vector2D> get_trajectory(const rcsc::Vector2D &target,
                                              const rcsc::Vector2D& pos,
                                              const rcsc::AngleDeg &body,
                                              const int& ptype_id) const;
};


AStarState*
simulate_next_state(const AStarState* state,
                    const AStarAction& action,
                    const rcsc::WorldModel& wm,
                    const double & dash_rate,
                    const double & player_decay);