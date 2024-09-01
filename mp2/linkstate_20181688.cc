#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 99999
#define MAX_NODE 100
#define MAX_LINE 100

typedef struct MESSAGE {
    int src;
    int dst;
    char txt[1000];
} MESSAGE;

typedef struct RT {
    int cost;
    int next_hop;
} RT;

MESSAGE msg[MAX_LINE];
RT routing_table[MAX_NODE][MAX_NODE];
int msg_num;
int node_num;
int input[MAX_NODE][MAX_NODE];

void print(FILE *fp);
void dijkstra(int start);

void print(FILE *fp) {
    int i, j;
    int src, dst;
    int path[MAX_NODE];
    int path_length = 0;
    int current_node;
    int total_cost;

    
    for (i = 0; i < node_num; i++) {
        for (j = 0; j < node_num; j++) {
            if (routing_table[i][j].cost < INF) {
                fprintf(fp, "%d %d %d\n", j, routing_table[i][j].next_hop, routing_table[i][j].cost);
            }
        }
        fprintf(fp, "\n");
    }

    
    for (i = 0; i < msg_num; i++) {
        src = msg[i].src;
        dst = msg[i].dst;
        path_length = 0;
        current_node = src;
        total_cost = routing_table[src][dst].cost;

        if (total_cost >= INF) {
            fprintf(fp, "from %d to %d cost infinite hops unreachable message %s\n", src, dst, msg[i].txt);
            continue;
        }

        while (current_node != dst) {
            path[path_length++] = current_node;
            current_node = routing_table[current_node][dst].next_hop;
        }
        path[path_length++] = dst;

        fprintf(fp, "from %d to %d cost %d hops ", src, dst, total_cost);
        for (j = 0; j < path_length-1; j++) {
            fprintf(fp, "%d ", path[j]);
        }
        fprintf(fp, "message %s\n", msg[i].txt);
    }
    fprintf(fp, "\n");
}

void dijkstra(int start) {
    int visited[MAX_NODE] = {0};
    int distance[MAX_NODE];
    int i, j, min_idx, min_dist;

    
    for (i = 0; i < node_num; i++) {
        distance[i] = INF;
        for (j = 0; j < node_num; j++) {
            routing_table[start][j].cost = INF;
            routing_table[start][j].next_hop = -1;
        }
    }

    distance[start] = 0;
    routing_table[start][start].cost = 0;
    routing_table[start][start].next_hop = start;

    for (i = 0; i < node_num; i++) {
        min_dist = INF;
        min_idx = -1;

        for (j = 0; j < node_num; j++) {
            if (!visited[j] && distance[j] < min_dist) {
                min_dist = distance[j];
                min_idx = j;
            }
        }

        if (min_idx == -1) break; 

        visited[min_idx] = 1;

        for (j = 0; j < node_num; j++) {
            if (!visited[j] && input[min_idx][j] < INF) {
                if (distance[min_idx] + input[min_idx][j] < distance[j]) {
                    distance[j] = distance[min_idx] + input[min_idx][j];
                    routing_table[start][j].cost = distance[j];
                    routing_table[start][j].next_hop = (start == min_idx) ? j : routing_table[start][min_idx].next_hop;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *in1, *in2, *in3, *out;
    int node1, node2, cost, i, j;
    char input1[20];
    char input2[1020];

    if (argc != 4) {
        fprintf(stderr, "usage: linkstate topologyfile messagesfile changesfile\n");
        exit(1);
    }

    
    in1 = fopen(argv[1], "r");
    if (in1 == NULL) {
        fprintf(stderr, "Error: open input file.\n");
        return 1;
    }

    in2 = fopen(argv[2], "r");
    if (in2 == NULL) {
        fprintf(stderr, "Error: open input file.\n");
        return 1;
    }

    in3 = fopen(argv[3], "r");
    if (in3 == NULL) {
        fprintf(stderr, "Error: open input file.\n");
        return 1;
    }

    out = fopen("output_ls.txt", "w");

    
    fscanf(in1, "%d", &node_num);
    for (i = 0; i < node_num; i++) {
        for (j = 0; j < node_num; j++) {
            if (i == j) {
                input[i][i] = 0;
            } else {
                input[i][j] = INF;
            }
        }
    }

    
    while (fgets(input1, sizeof(input1), in1)) {
        if (sscanf(input1, "%d %d %d", &node1, &node2, &cost) == 3) {
            input[node1][node2] = cost;
            input[node2][node1] = cost;
        }
    }

    
    while (fgets(input2, sizeof(input2), in2)) {
        input2[strcspn(input2, "\n")] = 0; 
        if (sscanf(input2, "%d %d %[^\n]", &msg[msg_num].src, &msg[msg_num].dst, msg[msg_num].txt) == 3) {
            msg_num++;
        }
    }

    
    for (i = 0; i < node_num; i++) {
        dijkstra(i);
    }
    print(out);

    
    while (fgets(input1, sizeof(input1), in3)) {
        if (sscanf(input1, "%d %d %d", &node1, &node2, &cost) == 3) {
            if (cost == -999) cost = INF;
            input[node1][node2] = cost;
            input[node2][node1] = cost;

            for (i = 0; i < node_num; i++) {
                dijkstra(i);
            }
            print(out);
        }
    }

    fclose(in1);
    fclose(in2);
    fclose(in3);
    fclose(out);

    return 0;
}
