// -*-c++-*-

/*!
  \file intercept_table.cpp
  \brief interception info holder Source File
*/

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "intercept_table_cyrus.h"
#include "player_intercept_cyrus.h"
#include "world_model.h"
#include "player_object.h"
#include "abstract_player_object.h"
#include "self_intercept_v13_cyrus.h"
#include "self_intercept_tackle.h"

#include <rcsc/time/timer.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/game_time.h>

#include <algorithm>

// #define DEBUG_PRINT

namespace rcsc {

namespace {
const int MAX_STEP = 50;
}

/*-------------------------------------------------------------------*/
/*!

*/
InterceptTableCyrus::InterceptTableCyrus( ) : InterceptTable()
{
    M_self_results_tackle.reserve( ( MAX_STEP + 1 ) * 2 );
    clear();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
InterceptTableCyrus::clear()
{
    InterceptTable::clear();
    M_self_step_tackle = 1000;
    M_self_exhaust_step_tackle = 1000;
    M_self_results_tackle.clear();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
InterceptTableCyrus::predictSelf(const WorldModel & wm)
{
    if ( wm.self().isKickable() )
    {
        dlog.addText( Logger::INTERCEPT,
                      "Intercept Self. already kickable. no estimation loop!" );
        M_self_step = 0;
        M_self_exhaust_step = 0;
        M_self_step_tackle = 1;
        M_self_exhaust_step_tackle = 1;
        return;
    }

    constexpr int max_step = 50;

    std::shared_ptr< InterceptSimulatorSelf > sim( new SelfInterceptV13() );
    sim->simulate( wm, max_step, M_self_results );

    std::shared_ptr< InterceptSimulatorSelf > sim_tackle( new SelfInterceptTackle() );
    sim->simulate( wm, max_step, M_self_results_tackle );
    //    SelfInterceptSimulator sim;
    //    sim.simulate( wm, max_step, M_self_results );

    if ( M_self_results.empty() )
    {
        std::cerr << wm.self().unum() << ' '
                  << wm.time()
                  << " Interecet Self cache is empty!"
                  << std::endl;
        dlog.addText( Logger::INTERCEPT,
                      "Intercept Self. Self cache is empty!" );
        // if self cache is empty,
        // reach point should be the inertia final point of the ball
        return;
    }
    else
    {
        // #ifdef SELF_INTERCEPT_USE_NO_SAVE_RECEVERY
        //     std::sort( M_self_results.begin(),
        //                M_self_results.end(),
        //                Intercept::Cmp() );
        //     M_self_results.erase( std::unique( M_self_results.begin(),
        //                                      M_self_results.end(),
        //                                      Intercept::Equal() ),
        //                         M_self_results.end() );
        // #endif

        int min_step = M_self_step;
        int exhaust_min_step = M_self_exhaust_step;

        for ( const Intercept & i : M_self_results )
        {
            if ( i.staminaType() == Intercept::NORMAL )
            {
                if ( i.reachStep() < min_step )
                {
                    min_step = i.reachStep();
                }
            }
            else if ( i.staminaType() == Intercept::EXHAUST )
            {
                if ( i.reachStep() < exhaust_min_step )
                {
                    exhaust_min_step = i.reachStep();
                }
            }
        }

        dlog.addText( Logger::INTERCEPT,
                      "Intercept Self. solution size = %d",
                      M_self_results.size() );

        M_self_step = min_step;
        M_self_exhaust_step = exhaust_min_step;

        //M_player_map.insert( std::pair< const AbstractPlayerObject *, int >( &(wm.self()), min_step ) );
    }
    if ( M_self_results_tackle.empty() )
    {
        std::cerr << wm.self().unum() << ' '
                  << wm.time()
                  << " Interecet Self cache tackle is empty!"
                  << std::endl;
        dlog.addText( Logger::INTERCEPT,
                      "Intercept Self. Self cache tackle is empty!" );
        // if self cache is empty,
        // reach point should be the inertia final point of the ball
    }else{
        int min_cycle = M_self_step_tackle;
        int exhaust_min_cycle = M_self_exhaust_step_tackle;

        const std::vector< Intercept >::iterator end = M_self_results_tackle.end();
        for ( std::vector< Intercept >::iterator it = M_self_results_tackle.begin();
              it != end;
              ++it )
        {
            if ( it->staminaType() == Intercept::NORMAL )
            {
                if ( it->reachStep() < min_cycle )
                {
                    min_cycle = it->reachStep();
                    break;
                }
            }
            else if ( it->staminaType() == Intercept::EXHAUST )
            {
                if ( it->reachStep() < exhaust_min_cycle )
                {
                    exhaust_min_cycle = it->reachStep();
                    break;
                }
            }
        }

        dlog.addText( Logger::INTERCEPT,
                      "Intercept Self Tackle. solution size = %d",
                      M_self_results_tackle.size() );

        M_self_step_tackle = min_cycle;
        M_self_exhaust_step_tackle = exhaust_min_cycle;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
InterceptTableCyrus::predictTeammate(const WorldModel & wm)
{
    int min_step = 1000;
    int second_min_step = 1000;

    if ( wm.kickableTeammate() )
    {
        M_teammate_step = 0;
        min_step = 0;
        M_first_teammate = wm.kickableTeammate();

        dlog.addText( Logger::INTERCEPT,
                      "Intercept Teammate. exist kickable teammate" );
        dlog.addText( Logger::INTERCEPT,
                      "---> set fastest teammate %d (%.1f %.1f)",
                      M_first_teammate->unum(),
                      M_first_teammate->pos().x, M_first_teammate->pos().y );
    }

    PlayerIntercept sim( wm.ball().pos(),
                                  ( wm.kickableOpponent() ? Vector2D( 0.0, 0.0 ) : wm.ball().vel() ) );

    for ( const PlayerObject * t : wm.teammatesFromBall() )
    {
        if ( t == wm.kickableTeammate() )
        {
            M_player_map[ t ] = 0;
            continue;
        }

        if ( t->posCount() >= 10 )
        {
            dlog.addText( Logger::INTERCEPT,
                          "Intercept Teammate %d.(%.1f %.1f) Low accuracy %d. skip...",
                          t->unum(),
                          t->pos().x, t->pos().y,
                          t->posCount() );
            continue;
        }

        int step = sim.simulate( wm, *t, false );
        if ( t->goalie() )
        {
            M_our_goalie_step = sim.simulate( wm, *t, true );
            if ( step > M_our_goalie_step )
            {
                step = M_our_goalie_step;
            }
        }

        dlog.addText( Logger::INTERCEPT,
                      "---> Teammate %d.(%.1f %.1f) step=%d",
                      t->unum(),
                      t->pos().x, t->pos().y,
                      step );

        if ( step < second_min_step )
        {
            second_min_step = step;
            M_second_teammate = t;

            if ( second_min_step < min_step )
            {
                std::swap( min_step, second_min_step );
                std::swap( M_first_teammate, M_second_teammate );
            }
        }

        M_player_map[ t ] = step;
    }

    if ( M_second_teammate && second_min_step < 1000 )
    {
        M_second_teammate_step = second_min_step;
    }

    if ( M_first_teammate && min_step < 1000 )
    {
        M_teammate_step = min_step;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
InterceptTableCyrus::predictOpponent(const WorldModel & wm)
{
    int min_step = 1000;
    int second_min_step = 1000;

    if ( wm.kickableOpponent() )
    {
        M_opponent_step = 0;
        min_step = 0;
        M_first_opponent = wm.kickableOpponent();

        dlog.addText( Logger::INTERCEPT,
                      "Intercept Opponent. exist kickable opponent" );
        dlog.addText( Logger::INTERCEPT,
                      "---> set fastest opponent %d (%.1f %.1f)",
                      M_first_opponent->unum(),
                      M_first_opponent->pos().x, M_first_opponent->pos().y );
    }

    PlayerIntercept sim( wm.ball().pos(),
                         ( wm.kickableOpponent() ? Vector2D( 0.0, 0.0 ) : wm.ball().vel() ) );

    for ( const PlayerObject * o : wm.opponentsFromBall() )
    {
        if ( o == wm.kickableOpponent() )
        {
            M_player_map[ o ] = 0;
            continue;
        }

        if ( o->posCount() >= 15 )
        {
            dlog.addText( Logger::INTERCEPT,
                          "Intercept Opponent %d.(%.1f %.1f) Low accuracy %d. skip...",
                          o->unum(),
                          o->pos().x, o->pos().y,
                          o->posCount() );
            continue;
        }

        int step = sim.simulate( wm, *o, false );
        if ( o->goalie() )
        {
            int goalie_step = sim.simulate( wm, *o, true );
            if ( goalie_step > 0
                 && step > goalie_step )
            {
                step = goalie_step;
            }
        }

        dlog.addText( Logger::INTERCEPT,
                      "---> Opponent.%d (%.1f %.1f) step=%d",
                      o->unum(),
                      o->pos().x, o->pos().y,
                      step );

        if ( step < second_min_step )
        {
            second_min_step = step;
            M_second_opponent = o;

            if ( second_min_step < min_step )
            {
                std::swap( min_step, second_min_step );
                std::swap( M_first_opponent, M_second_opponent );
            }
        }

        M_player_map[ o ] = step;
    }

    if ( M_second_opponent && second_min_step < 1000 )
    {
        M_second_opponent_step = second_min_step;
    }

    if ( M_first_opponent && min_step < 1000 )
    {
        M_opponent_step = min_step;
    }
}

}
