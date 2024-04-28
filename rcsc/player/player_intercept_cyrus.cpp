// -*-c++-*-

/*!
  \file player_intercept.cpp
  \brief intercept predictor for other players Source File
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

#include "player_intercept_cyrus.h"
#include "world_model.h"
#include "ball_object.h"
#include "player_object.h"
#include "cut_ball_calculator.h"
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/player_type.h>
#include <rcsc/soccer_math.h>

// #define DEBUG
// #define DEBUG2

namespace rcsc {

namespace {

/*-------------------------------------------------------------------*/
inline
    Vector2D
    get_pos( const PlayerObject & p )
{
    return ( p.heardPosCount() < p.seenPosCount()
                 ? p.heardPos()
                 : p.seenPos() );
}

/*-------------------------------------------------------------------*/
inline
    Vector2D
    get_vel( const PlayerObject & p )
{
    return ( p.velCount() < p.seenVelCount()
                 ? p.vel()
                 : p.seenVel() );
}

/*-------------------------------------------------------------------*/
inline
    double
    get_control_area( const PlayerObject & p,
                      const WorldModel & wm,
                      const bool goalie )
{
    if ( p.side() == wm.ourSide() )
    {
        return ( goalie
                     ? p.playerTypePtr()->reliableCatchableDist() - 0.2
                     : p.playerTypePtr()->kickableArea() - 0.2 );
    }

    return ( goalie
                 ? p.playerTypePtr()->reliableCatchableDist()
                 : p.playerTypePtr()->kickableArea() );
}

/*-------------------------------------------------------------------*/
inline
    int
    get_bonus_step( const PlayerObject & /*p*/,
                    const SideID /*our_side*/ )
{
    return 0;
    //    return p.side() == our_side
    //        // ? std::min( 3, static_cast< int >( std::ceil( std::min( p.heardPosCount(), p.seenPosCount() ) * 0.75 ) ) )
    //        // : std::min( 3, static_cast< int >( std::ceil( std::min( p.heardPosCount(), p.seenPosCount() ) * 0.75 ) ) );
    //        ? std::min( 3, std::min( p.heardPosCount(), p.seenPosCount() ) )
    //        : std::min( 3, std::min( p.heardPosCount(), p.seenPosCount() ) );
}

/*-------------------------------------------------------------------*/
inline
    int
    get_penalty_step( const PlayerObject & p )
{
    return ( p.isTackling()
                 ? std::max( 0, ServerParam::i().tackleCycles() - p.tackleCount() - 2 )
                 : 0 );
}

}

/*-------------------------------------------------------------------*/
/*!

*/
int
PlayerIntercept::simulate( const WorldModel & wm,
                                  const PlayerObject & player,
                                  const bool goalie ) const
{
    const PlayerType * ptype = player.playerTypePtr();

    if ( ! ptype )
    {
        std::cerr << __FILE__ << ' ' << __LINE__
                  << ": ERROR NULL player type." << std::endl;
        dlog.addText( Logger::INTERCEPT,
                      __FILE__": NULL player type. side=%c unum=%d",
                      side_char( player.side() ), player.unum() );
        return 1000;
    }

    const ServerParam & SP = ServerParam::i();

    const double pen_area_x = SP.pitchHalfLength() - SP.penaltyAreaLength();
    const double pen_area_y = SP.penaltyAreaHalfWidth();

    const PlayerData data( player,
                           *ptype,
                           get_pos( player ),
                           get_vel( player ),
                           get_control_area( player, wm, goalie ),
                           get_bonus_step( player, wm.ourSide() ),
                           get_penalty_step( player ) );

    const int min_step = estimateMinStep( data );
    const int max_step = M_ball_cache.size() - 1;

#ifdef DEBUG
    dlog.addText( Logger::INTERCEPT,
                  "Intercept Player %c %d (%.1f %.1f) - min=%d max=%d pos=(%.1f %.1f) bonus=%d penalty=%d",
                  side_char( player.side() ),
                  player.unum(),
                  player.pos().x, player.pos().y,
                  min_step, max_step,
                  data.pos_.x, data.pos_.y,
                  data.bonus_step_, data.penalty_step_ );
#endif

    if ( min_step > max_step )
    {
        return predictFinal( data );
    }

    for ( int total_step = min_step; total_step < max_step; ++total_step )
    {
        const Vector2D & ball_pos = M_ball_cache[total_step];
#ifdef DEBUG2
        dlog.addText( Logger::INTERCEPT,
                      "*** step=%d  ball(%.2f %.2f)",
                      total_step, ball_pos.x, ball_pos.y );
#endif

        if ( goalie
             && ( ball_pos.absX() < pen_area_x
                  || pen_area_y < ball_pos.absY() ) )
        {
            // never reach
#ifdef DEBUG2
            dlog.addText( Logger::INTERCEPT,
                          "--->cycle=%d goalie. out of penalty area. ball(%.2f %.2f)",
                          total_step, ball_pos.x, ball_pos.y );
#endif
            continue;
        }

        if ( std::pow( data.control_area_
                           + data.ptype_.realSpeedMax() * ( total_step + data.bonus_step_ - data.penalty_step_ )
                           + 0.5, 2 )
             < data.pos_.dist2( ball_pos ) )
        {
            // never reach
#ifdef DEBUG2
            dlog.addText( Logger::INTERCEPT,
                          "--->step=%d  never reach! ball(%.2f %.2f) dist=%.3f",
                          total_step, ball_pos.x, ball_pos.y,
                          data.pos_.dist( ball_pos ) );
#endif
            continue;
        }

        if ( canReachAfterTurnDashCyrus( data,
                                         ball_pos,
                                         total_step ) )
        {
#ifdef DEBUG
            dlog.addText( Logger::INTERCEPT,
                          "--->cycle=%d  Sucess! ball(%.2f %.2f)",
                          total_step, ball_pos.x, ball_pos.y );
#endif
            return total_step;
        }
    }

    if ( goalie
         && ( M_ball_cache.back().absX() < pen_area_x
              || pen_area_y < M_ball_cache.back().absY() ) )
    {
#ifdef DEBUG
        dlog.addText( Logger::INTERCEPT,
                      "FAILURE goalie. final. over the penalty area. bpos=(%.2f %.2f)",
                      M_ball_cache.back().x, M_ball_cache.back().y );
#endif
        return 1000;
    }

    return predictFinal( data );
}

bool
PlayerIntercept::canReachAfterTurnDashCyrus( const PlayerData & data,
                                                      const Vector2D & ball_pos,
                                                      const int total_step ) const
{
    /*
      TODO
      if ( canReachAfterOmniDash() )
      {
          return true;
      }
     */
    Vector2D pos = data.player_.pos();
    Vector2D vel = data.player_.vel();
    pos+=(vel * data.player_.playerTypePtr()->playerSpeedMax() / 0.4);
    int dash_cycle;
    int turn_cycle;
    int view_cycle;
    int n_step = CutBallCalculator().cycles_to_cut_ball(&data.player_,
                                                         ball_pos,
                                                         total_step,
                                                         false,
                                                         dash_cycle,
                                                         turn_cycle,
                                                         view_cycle,
                                                         pos,
                                                         vel,
                                                         data.player_.body().degree());

    int bonus_step = std::max( 0, data.bonus_step_ - turn_cycle );
    n_step -= bonus_step;
    n_step += data.penalty_step_;
    if( n_step <= total_step)
        return true;
    return false;
}
}