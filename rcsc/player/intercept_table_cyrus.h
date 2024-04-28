// -*-c++-*-

/*!
  \file intercept_table.h
  \brief interception info holder Header File
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

#ifndef RCSC_PLAYER_INTERCEPT_TABLE_CYRUS_H
#define RCSC_PLAYER_INTERCEPT_TABLE_CYRUS_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/game_time.h>
#include <rcsc/player/intercept.h>
#include <vector>
#include <map>

namespace rcsc {

class AbstractPlayerObject;
class PlayerObject;
class WorldModel;

/*-------------------------------------------------------------------*/

/*!
  \class Intercept
  \brief interception data
*/

/*-------------------------------------------------------------------*/

/*!
  \class InterceptTableCyrus
  \brief interception info holder for all players
*/
class InterceptTableCyrus {
private:

    //! reference to the WorldModel instance
    const WorldModel & M_world;

    //! last updated time
    GameTime M_update_time;

    //! ball inertia movement position cache
    std::vector< Vector2D > M_ball_cache;

    //! predicted min reach step for self without stamina exhaust
    int M_self_reach_step;
    int M_self_reach_cycle_tackle;
    //! predicted min reach step for self with stamina exhaust
    int M_self_exhaust_reach_step;
    int M_self_exhaust_reach_cycle_tackle;
    //! predicted min reach step for teammate
    int M_teammate_reach_step;
    //! predicted reach step for second fastest teammate
    int M_second_teammate_reach_step;
    //! predicted min reach step for teammate goalie
    int M_goalie_reach_step;
    //! predicted min reach step for opponent
    int M_opponent_reach_step;
    //! predicted reach step for second fastest opponent
    int M_second_opponent_reach_step;

    //! const pointer to the fastest ball gettable teammate player object
    const PlayerObject * M_fastest_teammate;
    //! const pointer to the second fastest ball gettable teammate player object
    const PlayerObject * M_second_teammate;
    //! const pointer to the fastest ball gettable opponent player object
    const PlayerObject * M_fastest_opponent;
    //! const pointer to the second fastest ball gettable opponent player object
    const PlayerObject * M_second_opponent;

    //! interception info cache for smart interception
    std::vector< Intercept > M_self_cache;
    std::vector< Intercept > M_self_cache_tackle;

    //! all players' intercept step container. key: pointer, value: step value
    std::map< const AbstractPlayerObject *, int > M_player_map;

    // not used
    InterceptTableCyrus() = delete;
    InterceptTableCyrus( const InterceptTableCyrus & ) = delete;
    InterceptTableCyrus & operator=( const InterceptTableCyrus & ) = delete;

public:
    /*!
      \brief init member variables, reserve cache vector memory
    */
    explicit
        InterceptTableCyrus( const WorldModel & world );

    /*!
      \brief destructor. nothing to do
    */
    virtual
        ~InterceptTableCyrus()
    { }

    /*!
      \brief recreate all interception info
    */
    void update(const WorldModel & world);

    /*!
      \brief set teammate intercept info mainly by heard info
      \param unum uniform number
      \param step interception step
     */
    void hearTeammate( const WorldModel & world,
                       const int unum,
                       const int step );

    /*!
      \brief set opponent intercept info mainly by heard info
      \param unum uniform number
      \param step interception step
     */
    void hearOpponent( const WorldModel & world,
                       const int unum,
                       const int step );

    /*!
      \brief get minimal ball gettable step for self without stamina exhaust
      \return step value to get the ball
    */
    int selfStep() const { return M_self_reach_step; }
    int selfStepTackle() const
    {
        return M_self_reach_cycle_tackle;
    }
    /*!
      \brief get minimal ball gettable step for self with stamina exhaust
      \return step value to get the ball
    */
    int selfExhaustStep() const { return M_self_exhaust_reach_step; }
    int selfExhaustStepTackle() const
    {
        return M_self_exhaust_reach_cycle_tackle;
    }
    /*!
      \brief get minimal ball gettable step for teammate
      \return step value to get the ball
    */
    int teammateStep() const { return M_teammate_reach_step; }

    /*!
      \brief get the ball access step for the second teammate
      \return step value to get the ball
    */
    int secondTeammateStep() const { return M_second_teammate_reach_step; }

    /*!
      \brief get the ball access step for the teammate goalie
      \return step value to get the ball
    */
    int ourGoalieStep() const { return M_goalie_reach_step; }

    /*!
      \brief get minimal ball gettable step for opponent
      \return step value to get the ball
    */
    int opponentStep() const { return M_opponent_reach_step; }

    /*!
      \brief get the ball access step for the second opponent
      \return step value to get the ball
    */
    int secondOpponentStep() const { return M_second_opponent_reach_step; }

    /*!
      \brief get the teammate object fastest to the ball
      \return const pointer to the PlayerObject.
      if not exist such a player, return NULL
    */
    const PlayerObject * firstTeammate() const { return M_fastest_teammate; }

    /*!
      \brief get the teammate object second fastest to the ball
      \return const pointer to the PlayerObject.
      if not exist such a player, return NULL
    */
    const PlayerObject * secondTeammate() const { return M_second_teammate; }

    /*!
      \brief get the oppnent object fastest to the ball
      \return const pointer to the PlayerObject.
      if not exist such a player, return NULL
    */
    const PlayerObject * firstOpponent() const { return M_fastest_opponent; }

    /*!
      \brief get the oppnent object second fastest to the ball
      \return const pointer to the PlayerObject.
      if not exist such a player, return NULL
    */
    const PlayerObject * secondOpponent() const { return M_second_opponent; }

    /*!
      \brief get self interception cache container
      \return const reference to the interception info container
    */
    const std::vector< Intercept > & selfResults() const
    {
        return M_self_cache;
    }

    const std::vector< Intercept > & selfResultsTackle() const
    {
        return M_self_cache_tackle;
    }
    /*!
      \brief get all players' intercept step container.
      \return map container. key: pointer, value: step value
     */
    const std::map< const AbstractPlayerObject *, int > & playerMap() const
    {
        return M_player_map;
    }

private:
    /*!
      \brief clear all cached data
    */
    void clear();

    /*!
      \brief create cache of future ball status
    */
    void createBallCache();

    /*!
      \brief predict self interception
    */
    void predictSelf();

    /*!
      \predict teammate interception
    */
    void predictTeammate();

    /*!
      \predict opponent interception
    */
    void predictOpponent();
};

}

#endif