#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <set>
#include <map>
#include <unordered_map>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include "params.h"
#include "CuckooSketchPro.h"
#include "CuckooSketchFlag.h"

using namespace std;
static map <string ,int> B,C;
static struct node {string x;int y;} p[32000005];
static string s[32000005];
static char tmp[105];
static int cmp(node i,node j) {return i.y>j.y;}
static double tar_prec = 0.95;

static std::vector<std::string> func_names;
static std::map<std::string, int> AAE;
static std::map<std::string, double> ARE;
static std::map<std::string, int> _sum;
static std::map<std::string, double> _throughput;
static std::vector<sketch::BaseSketch*> func;

int main(int argc, char** argv)
{
    int MEM=400,K=1000;
    int c;
    char dataset[40]={'\0'};
    while((c=getopt(argc, argv, "d:m:k:"))!=-1) {
        switch(c) {
            case 'd':
                strcpy(dataset,optarg);
                break;
            case 'm':
                MEM=atoi(optarg);
                break;
            case 'k':
                K=atoi(optarg);
                break;
        }
    }
    cout<<"MEM="<<MEM<<"KB"<<endl<<"Find top "<<K<<endl<<endl;
    cout<<"preparing all algorithms"<<endl;
    int m=MAX_INSERT;  // the number of flows

    // Inserting
	timespec time1, time2;
	long long resns;
	char default_dataset[40]="../../real_data/1.dat";
	if(dataset[0]=='\0') strcpy(dataset, default_dataset);
	cout<<"dataset: "<<dataset<<endl<<endl;
	ifstream fin(dataset, ios::in|ios::binary);
	if(!fin) {printf("Dataset not exists!\n");return -1;}
    std::vector<std::string> flows;
    std::unordered_map<std::string, int> flow_info;
	for (int i = 1; i <= m; i++)
	{
		fin.read(tmp, KEY_LEN);
		tmp[KEY_LEN]='\0';
        flows.push_back(tmp);
        flow_info[flows.back()]++;
        if (fin.eof()) {
            m = i;
            break;
        }
	}

    // Calculating sorted flow
    std::vector<pair<int, std::string>> flow_cnt;
    for (auto &[str, cnt] : flow_info) {
        flow_cnt.push_back({cnt, str});
    }
    K = min(K, static_cast<int>(flow_cnt.size()));
    sort(flow_cnt.begin(), flow_cnt.end(), [](std::pair<int, std::string> &x, std::pair<int, std::string> &y) {
        return x.first > y.first;
    });
    int topk_freq = flow_cnt[K - 1].first;
    std::unordered_map<std::string, int> topk_flow;
    int acceptable_error_range = K * 0.02;
    for (int i = 0; i < flow_cnt.size() && i < K + acceptable_error_range; i++) {
        topk_flow[flow_cnt[i].second] = flow_cnt[i].first;
    }
    for (int i = K; i < flow_cnt.size() && flow_cnt[i].second == flow_cnt[K - 1].second; i++) {
        topk_flow[flow_cnt[i].second] = flow_cnt[i].first;
    }

    std::cout << "real insert flow is " << m << std::endl;
    std::cout << "total diff flow size is " << flow_cnt.size() << std::endl;
    std::cout << "real k is " << K << std::endl;
    std::cout << "real top-k freq is: " << topk_freq << std::endl;

    // preparing Sketch

    // preparing CuckooSketchPro
    func.push_back(new sketch::cuckoosketchpro::CuckooSketchPro(topk_freq, K, MEM));

    // preparing CuckooSketchFlag
    func.push_back(new sketch::cuckoo_flag::CuckooSketchFlag(topk_freq, MEM));

    // prepare clear
    for (auto &iter : func) {
        func_names.push_back(iter->get_name());
        iter->clear();
    }

	printf("*************throughput************\n");
    for (auto &sketch_func : func) {
        clock_gettime(CLOCK_MONOTONIC, &time1);
        for (int i = 1; i <= flows.size(); i++) {
            sketch_func->Insert(flows[i]);
        }
        clock_gettime(CLOCK_MONOTONIC, &time2);
	    resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
        double throughput = (double)1000.0 * m / resns;
        printf("throughput of %s (insert): %.6lf Mips\n", sketch_func->get_name().c_str(), throughput);
        _throughput[sketch_func->get_name()] = throughput;
    }

    for (auto &sketch_func : func) {
        std::cout << sketch_func->get_name() << " work" << std::endl;;
        sketch_func->work();
    }

    for (auto &sketch_func : func) {
        std::cout << sketch_func->get_name() << " calculating" << std::endl;
    }

    // Calculating PRE, ARE, AAE
    std::vector<std::vector<int>> record;
    record.push_back({});
    for (auto &iter : flow_cnt) {
        record.back().push_back(iter.first);
    }

    cout << "Calculating\n" << endl;
    for (auto &sketch_func : func) {
        std::cout << sketch_func->get_name() << " calculating" << std::endl;
        std::string str;
        record.push_back({});
        std::set<std::string> exist_flow;
        int num;
        for (int i = 0; i < K; i++) {
            auto [str, num] = sketch_func->Query(i);
            record.back().push_back(num);
            AAE[sketch_func->get_name()] += abs(flow_info[str] - num);
            ARE[sketch_func->get_name()] += abs(flow_info[str] - num) / (flow_info[str] + 0.0);
            if (topk_flow.count(str) && !exist_flow.count(str)) {
                _sum[sketch_func->get_name()]++;
            }
            exist_flow.insert(str);
        }

        printf("%s:\n", sketch_func->get_name().c_str());
        printf("\tAccepted: %d/%d %.10f\n", _sum[sketch_func->get_name()], K, (_sum[sketch_func->get_name()] / (K + 0.0)));
        printf("\tARE: %.10f\n", ARE[sketch_func->get_name()] / K);
        printf("\tAAE: %.10f\n\n", AAE[sketch_func->get_name()] / (K + 0.0));
    }
	return 0;
}
