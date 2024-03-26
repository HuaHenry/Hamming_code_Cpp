//
//  main.cpp
//  Hamming_code
//
//  Created by 华洲琦 on 2024/3/26.
//

#include <iostream>
#include <vector>
#include <random>
using namespace std;

//计算校验码位数
int cal_hamming_bits(int n){
    int x = 0;
    while (true) {
        int value = pow(2, x) - 1;
        if (value >= n + x) {
            return x;
        }
        x++;
    }
}

//判断是否是2的幂次
bool isPowerOfTwo(int n) {
    // 使用位运算判断是否只有一位是 1
    return (n & (n - 1)) == 0 ;
}

//海明码非校验码编码
void hamming_meta_encoding(vector<int>* trans,string meta_data){
    int index = 0;
    for(int i=1;i<trans->size();i++){
        if(isPowerOfTwo(i)) continue;   //校验码位置
        else{
            (*trans)[i] = meta_data[index++]-'0';
        }
    }
}

//海明码校验码编码
void hamming_check_encoding(vector<int>* trans){
    int index = 1;
    for(int i=1;i<trans->size();i++){
        if(isPowerOfTwo(i)){
            int mask = 1 << ((index++)-1);      //创建一个掩码，只有第 n 位是 1，其余位都是 0
            int res = 0;            //连续异或的结果值
            for(int j=1;j<trans->size();j++){
                if((j & mask) != 0 && j != i){
                    res ^= (*trans)[j];
                }
            }
            (*trans)[i] = res;
        }
    }
}

//随机产生一位错误
void random_wrong(vector<int>& trans){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);      //均匀分布，范围为 [0, 1]
    std::uniform_int_distribution<> dis_wa(1, int(trans.size())-1);
    if(dis(gen)){
        //传输过程中产生错误
        int pos = dis_wa(gen);
        trans[pos] ^= 1;
    }
}

//传输后的校验/纠错过程
void check_correct(vector<int>& trans,int hamming_bits){
    vector<int> wrong(hamming_bits+1,0);        //错误位的二进制代码
    bool wa_flag = false;
    for(int i=1;i<=hamming_bits;i++){
        int mask = 1 << (i-1);
        int res = 0;
        for(int j=1;j<trans.size();j++){
            if((j & mask) != 0){
                res ^= trans[j];
            }
        }
        if(res!=0){
            wrong[i] = 1;
            wa_flag = true;
        }
    }
    if(wa_flag){
        
        int tmp = 1;                            //2的幂次
        int res = 0;                            //错位10进制位
        for(int i=1;i<=hamming_bits;i++){
            res += tmp*wrong[i];
            tmp *= 2;
        }
        trans[res] = 1^trans[res];
        cout<<"检测到第"<<res<<"位错误😡"<<endl;
        cout<<"修正后的海明码：";
        for(int i=1;i<trans.size();i++){
            cout<<trans[i];
        }
        cout<<endl;
    }
    else{
        cout<<"经校验，不存在一位错误😄"<<endl;
    }
}


int main(int argc, const char * argv[]) {
    string meta_data;               //待传输的元数据
    cin>>meta_data;
    
    int n = int(meta_data.size());
    cout<<"传输信息："<<n<<"位二进制位"<<endl;
    int hamming_bits = cal_hamming_bits(n);
    cout<<"需要海明校验码"<<hamming_bits<<"位"<<endl;
    int bits_all = hamming_bits + n;
    cout<<"传输编码共"<<bits_all<<"位"<<endl;
    
    vector<int> deliver_data(bits_all+1,8);         //传输数据 = 元数据 + 校验位 (从index=1开始存储)
    hamming_meta_encoding(&deliver_data,meta_data); //数据编码
    hamming_check_encoding(&deliver_data);          //校验编码
    
    cout<<endl<<"计算校验位后的海明码为：";
    for(int i=1;i<bits_all+1;i++){
        cout<<deliver_data[i];
    }
    cout<<endl;
    
    random_wrong(deliver_data);                     //随机产生/不产生一位错误
    cout<<endl<<"传输后的海明码为：";
    for(int i=1;i<bits_all+1;i++){
        cout<<deliver_data[i];
    }
    cout<<endl<<endl;
    
    check_correct(deliver_data,hamming_bits);       //差错和纠正
}
