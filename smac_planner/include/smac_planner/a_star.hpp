// Copyright (c) 2020, Samsung Research America
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License. Reserved.

#ifndef SMAC_PLANNER__A_STAR_HPP_
#define SMAC_PLANNER__A_STAR_HPP_

#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <queue>
#include <utility>

#include "smac_planner/node.hpp"
#include "smac_planner/types.hpp"

namespace smac_planner
{

// https://www.cs.cmu.edu/~motionplanning/lecture/AppH-astar-dstar_howie.pdf
// https://en.wikipedia.org/wiki/A*_search_algorithm
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathPlanning/AStar/a_star.py

// add tests

/**
 * @class smac_planner::AStarAlgorithm
 * @brief An A* implementation for planning in a costmap
 */
class AStarAlgorithm
{
public:
  /**
   * @brief A constructor for nav2_planner::PlannerServer
   * @param neighborhood The type of neighborhood to use for search (4 or 8 connected)
   */
  explicit AStarAlgorithm(const Neighborhood & neighborhood);

  /**
   * @brief A destructor for smac_planner::AStarAlgorithm
   */
  ~AStarAlgorithm();

  /**
   * @brief Initialization of the planner with defaults
   * @param travel_cost Cost to travel from adjacent cells to another
   * @param allow_unknown Allow search in unknown space, good for navigation while mapping
   * @param max_iterations Maximum number of iterations to use while expanding search
   */
  void initialize(
    const float & travel_cost,
    const bool & allow_unknown,
    const float & max_iterations);

  /**
   * @brief Creating path from given costmap, start, and goal
   * @param path Reference to a vector of indicies of generated path
   * @return if plan was successful
   */
  bool createPath(IndexPath & path);

  /**
   * @brief Set the costs of the graph
   * @param x The total number of cells in the X direction
   * @param y The total number of cells in the X direction
   * @param costs unsigned char * to the costs in the graph
   */
  void setCosts(
    const unsigned int & x,
    const unsigned int & y,
    unsigned char * costs);

  /**
   * @brief Set the goal for planning, as a cell index
   * @param value The cell index of the goal
   */
  void setGoal(const unsigned int & value);

  /**
   * @brief Set the starting pose for planning, as a cell index
   * @param value The cell index of the start
   */
  void setStart(const unsigned int & value);

  /**
   * @brief Set the starting pose for planning, as a cell index
   * @param node Node pointer to the goal node to backtrace
   * @param path Reference to a vector of indicies of generated path
   * @return whether the path was able to be backtraced
   */
  bool backtracePath(Node * & node, IndexPath & path);

private:
  /**
   * @brief Get pointer reference to starting node
   * @return Node pointer reference to starting node
   */
  Node * & getStart();

  /**
   * @brief Get pointer reference to goal node
   * @return Node pointer reference to goal node
   */
  Node * & getGoal();

  /**
   * @brief Get pointer to next goal in open set
   * @return Node pointer reference to next heuristically scored node
   */
  Node * getNode();

  /**
   * @brief Get pointer to next goal in open set
   * @param cost The cost to sort into the open set of the node
   * @param node Node pointer reference to add to open set
   */
  void addNode(const float cost, Node * & node);

  /**
   * @brief Check if this node is the goal node
   * @param node Node pointer to check if its the goal node
   * @return if node is goal
   */
  bool isGoal(Node * & node);

  /**
   * @brief Check if this cell is valid
   * @param i Node index
   * @param cell_it Iterator reference of the cell
   * @return whether this cell is valid and collision free
   */
  bool isCellValid(const unsigned int & i, Graph::iterator & cell_it);

  /**
   * @brief Get a vector of valid node pointers from relative locations
   * @param lookup_table Lookup table of values around cell to query
   * @param cell Node index
   * @return Vector of node pointers to valid cells
   */
  NodeVector getValidCells(
    const std::vector<int> & lookup_table,
    const unsigned int & cell);

  /**
   * @brief Get cost of a cell from graph
   * @param cell Node index
   * @return Reference to cell cost
   */
  float & getCellCost(const unsigned int & cell);

  /**
   * @brief Get cost of traversal between nodes
   * @param cell Node index current
   * @param cell Node index of new
   * @return Reference traversal cost between the cells
   */
  float & getTraversalCost(const unsigned int & lastCell, const unsigned int & cell);

  /**
   * @brief Get cost of heuristic of node
   * @param cell Cell index current
   * @param cell Cell index of new
   * @return Heuristic cost between the cells
   */
  float getHeuristicCost(const unsigned int & cell);

  /**
   * @brief Get a vector of neighbors around node
   * @param cell Node index
   * @return Vector of node pointers to neighbors
   */
  NodeVector getNeighbors(const unsigned int & cell);

  /**
   * @brief Check if inputs to planner are valid
   * @return Are valid
   */
  bool areInputsValid();

  /**
   * @brief Get maximum number of iterations to plan
   * @return Maximum iterations parameter
   */
  unsigned int & getMaxIterations() {return max_iterations_;}

  /**
   * @brief Get size of graph in X
   * @return Size in X
   */
  unsigned int & getSizeX() {return x_size_;}

  /**
   * @brief Get size of graph in Y
   * @return Size in Y
   */
  unsigned int & getSizeY() {return y_size_;}

  /**
   * @brief Get node coordinates from index
   * @param index Index of node
   * @return pair of XY coordinates
   */
  std::pair<unsigned int, unsigned int> getCoords(const unsigned int & index)
  {
    const unsigned int x = index % getSizeX();
    const unsigned int y = index / getSizeX();
    return std::pair<unsigned int, unsigned int>(x, y);
  }

  /**
   * @brief Clear hueristic queue of nodes to search
   */
  void clearQueue();

  float travel_cost_;
  bool traverse_unknown_;
  unsigned int max_iterations_;
  unsigned int x_size_;
  unsigned int y_size_;

  Coordinates goal_coordinates_;
  Node * start_;
  Node * goal_;

  std::unique_ptr<Graph> graph_;
  std::unique_ptr<NodeQueue> queue_;

  Neighborhood neighborhood_;
};

}  // namespace smac_planner

#endif  // SMAC_PLANNER__A_STAR_HPP_