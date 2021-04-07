#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <cassert>

using namespace std;
#define MAX_VERTEXES 1000
int edges[MAX_VERTEXES][MAX_VERTEXES];
bool undirectedEdges[MAX_VERTEXES][MAX_VERTEXES];
struct cas
{
    cas(bool _res, int _expVal, int _newVal) : res(_res), expVal(_expVal), newVal(_newVal), used(false) {}
    bool res;
    int expVal, newVal;
    bool used;
};

int degIn[MAX_VERTEXES];
int degOut[MAX_VERTEXES];

bool visited[MAX_VERTEXES];
void dfs(int s, int vertexCnt){
    if(visited[s])
        return;
    visited[s] = true;
    for(int i = 0 ; i < vertexCnt ; i++)
        if(undirectedEdges[s][i])
            dfs(i, vertexCnt);
}

vector<int> extractCircuit(vector< vector<int> >& adj, int startVertex)
{
    unordered_map<int,int> edge_count;
  
    for (int i=0; i<adj.size(); i++)
    {
        //find the count of edges to keep track
        //of unused edges
        edge_count[i] = adj[i].size();
    }
  
    // Maintain a stack to keep vertices
    stack<int> curr_path;
  
    // vector to store final circuit
    vector<int> circuit;
    if (!adj.size())
        return circuit; //empty graph
  
  
    // start from any vertex
    curr_path.push(startVertex);
    int curr_v = startVertex; // Current vertex
  
    while (!curr_path.empty())
    {
        // If there's remaining edge
        if (edge_count[curr_v])
        {
            // Push the vertex
            curr_path.push(curr_v);
  
            // Find the next vertex using an edge
            int next_v = adj[curr_v].back();
  
            // and remove that edge
            edge_count[curr_v]--;
            adj[curr_v].pop_back();
  
            // Move to next vertex
            curr_v = next_v;
        }
  
        // back-track to find remaining circuit
        else
        {
            circuit.push_back(curr_v);
  
            // Back-tracking
            curr_v = curr_path.top();
            curr_path.pop();
        }
    }
  
    return circuit;
}
int getValByIdx(int _idx, const map<int, int>& N){
    for (const auto& [value, idx] : N) {
        if(_idx == idx)
            return value;
    }
    return -1;
}
int main(){
    // считывание операций 
    int nCas, init, result;
    cin >> nCas >> init >> result;

    map<int, int> N;
    N.insert(make_pair(init, N.size()));
    N.insert(make_pair(result, N.size()));

    vector<cas> ops;
    for(int i = 0 ; i < nCas ; i++){
        bool resOp;
        int expVal, newVal;
        cin >> resOp >> expVal >> newVal;
        ops.push_back(cas(resOp, expVal, newVal));

        // конструируем N: собираем все уникальные значения
        N.insert(make_pair(expVal, N.size()));
        N.insert(make_pair(newVal, N.size()));
    }
    for(int i = 0 ; i < nCas ; i++){
        // конструируем A: добавляем ребра a->b для успешных операций и петли для неудачных
        if(ops[i].res){
            edges[N.find(ops[i].expVal)->second][N.find(ops[i].newVal)->second]++;
        }
        else{
            // ищем первую вершину, в которой значение != ops[i].expVal
            for (const auto& [value, idx] : N) {
                if(value != ops[i].expVal){
                    edges[idx][idx]++;
                    break;
                }
            }
        }
    }

    // считаем степени входа-выхода у вершин
    for(int i = 0 ; i < N.size() ; i++){
        for(int j = 0 ; j < N.size() ; j++){
            if(edges[i][j] > 0){
                degOut[i] += edges[i][j];
                degIn[j] += edges[i][j];
            }
        }
    }

    
    // проверка условий на степени входа-выхода у вершин:
    // > если для всех вершин deg+ == deg-, и init == result и (init принадлежит единственной комп. связности или ребер нет)
    // > если для всех вершин кроме двух, то init=a, result=b
    // дополнительные условия init==result / init==a, result==b проверяются позднее
    int v1 = -1, v2 = -1;
    bool bad = false;
    for(int i = 0 ; i < N.size() && !bad ; i++){
        if(degIn[i] == degOut[i])
            continue;
        else if(degIn[i] == degOut[i]+1){
            if(v2 == -1)
                v2 = i;
            else
                bad = true;
        }
        else if(degOut[i] == degIn[i]+1){
            if(v1 == -1)
                v1 = i;
            else
                bad = true;
        }
        else{
            bad = true;
        }
    }
    // если мы получили только одну из вершин a, b - все плохо
    if((v1 == -1 && v2 != -1) || (v1 != -1 && v2 == -1)){
        bad = true;
    }

    // проверка принадлежности всех ребер к одной компоненте связности
    // строим неориентированный граф
    for(int i = 0 ; i < N.size() ; i++){
        for(int j = 0 ; j < N.size() ; j++){
            if(edges[i][j] > 0){
                undirectedEdges[i][j] = true;
                undirectedEdges[j][i] = true;
            }
        }
    }

    // запускаем обход из неизолированной вершины
    for(int i = 0 ; i < N.size() ; i++){
        if(degIn[i] > 0 || degOut[i] > 0){
            dfs(i, N.size());
            break;
        }
    }
    // если после обхода мы сможем найти неизолированную непосещенную вершину - 
    // всё плохо, у нас есть другие компоненты связности с ребрами
    for(int i = 0 ; i < N.size() ; i++){
        if((degIn[i] > 0 || degOut[i] > 0) && !visited[i]){
            bad = true;
            break;
        }
    }
    // если к этому моменту bad = false, то у нас точно одна компонента слабой связности с ребрами
    // а также либо для всех вершин degOut==degIn, либо мы нашли a и b, для которых инвариант описан раньше
    if(v1 == -1 && v2 == -1){
        // у нас Эйлеров цикл, надо проверить, что init лежит в компоненте связности (или ребер нет) и init == result
        if(!((visited[N.find(init)->second] || nCas == 0) && init == result)){
            bad = true;
        }
    }
    else{
        // у нас Эйлеров путь, надо проверить v1 == init.idx, v2 == result.idx
        assert(visited[N.find(init)->second] && visited[N.find(result)->second]);
        if(!(v1 == N.find(init)->second && v2 == N.find(result)->second)){
            bad = true;
        }
    }

    if(bad){
        cout << "Not a serializable history\n";
        return 0;
    }

    // построение эйлерова пути
    // загоняем в списки инцидентности
    vector<vector<int>> adj_lists(N.size());
    for(int i = 0 ; i < N.size() ; i++){
        for(int j = 0 ; j < N.size() ; j++){
            while(edges[i][j] > 0){
                edges[i][j]--;
                adj_lists[i].push_back(j);
            }
        }
    }
    cout << "Serializable history =)\n";
    if(nCas > 0)
        cout << "Sequentional order:\n";
    
    // строим цикл по алгоритму Хирхольцера
    vector<int> circuit = extractCircuit(adj_lists, N.find(init)->second);
    for(int i = circuit.size()-1 ; i >= 1 ; i--){
        // реконструируем операцию
        // a и b - индексы вершин в графе
        int a = circuit[i], b = circuit[i-1];
        // expVal и newVal - значения, которые лежат по этим индексам
        int expVal = getValByIdx(a, N), newVal = getValByIdx(b, N);
        for(int j = 0 ; j < nCas ; j++){
            // по построению графа найдется успешная cas-операция, соответствующая данным expVal и newVal
            // или, если a==b, у нас есть возможность взять неудачную cas-операцию, в которой ops[].expVal != expVal
            if(!ops[j].used && ops[j].res && ops[j].expVal == expVal && ops[j].newVal == newVal){
                ops[j].used = true;
                cout << ops[j].res << " " << ops[j].expVal << " " << ops[j].newVal << "\n";
                break;
            }
            if(!ops[j].used && !ops[j].res && ops[j].expVal != expVal && a == b){
                ops[j].used = true;
                cout << ops[j].res << " " << ops[j].expVal << " " << ops[j].newVal << "\n";
                break;
            }
        } 
    }
}