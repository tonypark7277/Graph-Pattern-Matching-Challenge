/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include <chrono>

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  
  std::cout << "t " << query.GetNumVertices() << "\n";

  // implement your code here.
  Vertex num_vertices = query.GetNumVertices();
  Vertex visited[num_vertices];
  int state_number = 0;
  Vertex state[num_vertices];     // state[i] = i 번째에 선택된 cs를 저장한다
  Vertex next_vertex = 0;         // matched에 추가될 다음 vertex
  Vertex matched[num_vertices];   // 현재까지 어느 vertex들이 선택됐는지 저장하는 vector
  std::vector<Vertex> extendable_candidate[num_vertices];
  std::vector<std::vector<Vertex>> extendable_vertex;

  for (int i = 0; i < num_vertices; i++){
    visited[i] = 0;
  }

  // mathced와 state를 초기화 하는 작업
  for (int i = 0; i < num_vertices; i++){
    matched[i] = 0;
    state[i] = -1;
  }
  Vertex min = 0;
  for (int i = 0; i < num_vertices; i++){
    if ( cs.GetCandidateSize(i) == cs.GetCandidateSize(min)){
      if (query.GetDegree(i) < query.GetDegree(min)){
        min = i;
      }
    }
    if ( cs.GetCandidateSize(i) < cs.GetCandidateSize(min)){
      min = i;
    }
  }
  state[0] = min;
  int count  = 0;
  
  // extendable_vertex를 초기화 하는 작업
  std::vector<Vertex> vertex_set;
  for (int index = query.GetNeighborStartOffset(state[0]); index < (int) query.GetNeighborEndOffset(state[0]); index++){
    vertex_set.push_back(query.GetNeighbor(index));
  }
  extendable_vertex.push_back(vertex_set);

  // extendable_candidate를 초기화 하는 작업
  for (int i = 0; i < num_vertices; i++){
    std::vector<Vertex> candidate ((int) cs.GetCandidateSize(i));
    candidate.clear();
    extendable_candidate[i] = candidate;
  }

  std::vector<Vertex> candidate_set;
  for (int i = 0; i < (int) cs.GetCandidateSize(state[0]); i++){
    candidate_set.push_back(cs.GetCandidate(state[0], i));
  }
  extendable_candidate[state[0]] = candidate_set;
  
  while (state_number >= 0 && count < 100000){    
    // i번째 candidate set의 모든 원소를 돌았을 경우 n-1 번째 candidate set으로 돌아가 다음 원소를 선택한다.
    if ((int) extendable_candidate[state[state_number]].size() == visited[state_number] ){
      visited[state_number] = 0;
      extendable_candidate[state[state_number]].clear();
      extendable_vertex.pop_back();
      state_number -= 1;
      continue;
    }

    // n 번째 vertex까지 탐색을 완료한 케이스로 각 결과를 출력한다.
    if (state_number == num_vertices-1){
      // extendable_candidate에서 다음으로 탐색할 vertex를 불러와 확인한다.
      std::vector<Vertex> temp = extendable_candidate[state[state_number]];
      next_vertex = temp[visited[state_number]];
      matched[state[state_number]] = next_vertex;
      
      // 결과를 출력하는 부분
      std::cout << "a ";
      for (int i = 0; i < num_vertices; i++){
        std::cout << matched[i] << " ";
      }
      std::cout << std::endl;
      visited[state_number] += 1;
      count += 1;
    }
    // 하위의 모든 candidate set을 탐색하고 i 번째 candidate로 돌아온 case로
    // matching할 다음 vertex를 찾고, 탐색할 i+1 번째 candidate set을 찾는다.
    else{

      // 구한 next_vertex가 현재의 extendable_candidate에 있는지 확인한다.
      //std::cout << "Here choosing next candidate" << std::endl;
      std::vector<Vertex> temp = extendable_candidate[state[state_number]];
      next_vertex = temp[visited[state_number]];
      matched[state[state_number]] = next_vertex;

      // 우선 extendable_vertex의 첫번째 원소에 해당하는 candidate set에서
      // extendable candidate를 뽑아내 저장한다.
      std::vector<Vertex> vertex_set = extendable_vertex.back();
      Vertex min_vertex = vertex_set.front();
    

      std::vector<Vertex> min_temp;
      int size = cs.GetCandidateSize(min_vertex);
      for (int offset = 0; offset < size; offset++){
        Vertex candidate = cs.GetCandidate(min_vertex, offset);
        bool isIncluded = false;
        bool isExtendable = true;
        for (int index = 0; index <= state_number; index ++){
          Vertex query_vertex = state[index];
          Vertex graph_vertex = matched[state[index]];
          if (graph_vertex == candidate){
            isIncluded = true;
            break;
          }
          if (query.IsNeighbor(query_vertex, min_vertex)){
            if (!data.IsNeighbor(graph_vertex, candidate)){
              isExtendable = false;
              break;
            }
          }
        }
        if (isIncluded) { continue; }
        if (isExtendable){
          min_temp.push_back(candidate);
        }
      }

      // 만약 extendable_vertex가 하나밖에 없을 경우, temp의 size가 0이 아니라면 
      // 다음 과정으로 넘어가고, 0이라면 이전 state로 돌아간다.
      if (min_temp.size() == 0){
        visited[state_number] += 1;
        continue;
      }
      
      bool isRightPath = true;

      // extendable_vertex가 여러개 있을 경우, extendable_candidate가 가장 적은 vertex를 고른다.
      for (int i = 1; i < (int) vertex_set.size(); i++){
        // 새로운 vertex 하나를 불러온다.
        Vertex new_vertex = vertex_set[i];
        std::vector<Vertex> temp;
        int new_size = cs.GetCandidateSize(new_vertex);
        for (int offset = 0; offset < new_size; offset++){
          Vertex candidate = cs.GetCandidate(new_vertex, offset);
          bool isIncluded = false;
          bool isExtendable = true;
          
          for (int index = 0; index <= state_number; index ++){
            Vertex query_vertex = state[index];
            Vertex graph_vertex = matched[state[index]];
            if (graph_vertex == candidate){
              isIncluded = true;
              break;
            }
            if (query.IsNeighbor(query_vertex, new_vertex)){
              if (!data.IsNeighbor(graph_vertex, candidate)){
                isExtendable = false;
                break;
              }
            }
          }
          if (isIncluded) { continue; }
          if (isExtendable){
            temp.push_back(candidate);
          }
        }
        if (temp.size() == 0){
          isRightPath = false;
          break;
        }
        if (min_temp.size() == temp.size()){
          if (query.GetDegree(min_vertex) > query.GetDegree(new_vertex)){
            min_vertex = new_vertex;
            min_temp = temp;
          }
        }
        if (min_temp.size() > temp.size()){
          min_vertex = new_vertex;
          min_temp = temp;
        }
      }
      
      // 가장 갯수가 적은 extendable한 candidate의 개수가 0이라면 이전 단계로 돌아간다.
      if (!isRightPath){
        visited[state_number] += 1;
        continue;
      }
      // 아니라면 extendable_vertex를 업데이트하고 다음 단계로 넘어간다.
      else{
        state[state_number + 1] = min_vertex;
        extendable_candidate[state[state_number + 1]] = min_temp;
        visited[state_number] += 1;

        // extendable_vertex 업데이트
        // 우선 다음 vertex로 지정된 min_vertex를 extendable_vertex에서 제거한다.
        std::vector<Vertex>::iterator iter;
        std::vector<Vertex> new_vertex_set;
        for (iter = vertex_set.begin(); iter != vertex_set.end(); iter++){
          if (*iter != min_vertex){
            new_vertex_set.push_back(*iter);
          }          
        }
        for (int index = query.GetNeighborStartOffset(min_vertex); index < (int) query.GetNeighborEndOffset(min_vertex); index++){
          Vertex neighbor = query.GetNeighbor(index);
          bool included = false;
          for (int i = 0; i <= state_number; i++){
            if (neighbor == state[i]) {
              included = true;
              break;
            }
          }
          if (included) {continue;}
          for (int i = 0; i < (int) new_vertex_set.size(); i++){
            if (new_vertex_set[i] == neighbor){
              included = true;
              break;
            }
          }
          if (included) {continue;}
          new_vertex_set.push_back(neighbor);
        }
        extendable_vertex.push_back(new_vertex_set);
        state_number += 1;
      }
    }
  }
}
