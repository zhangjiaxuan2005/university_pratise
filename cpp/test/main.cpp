#include <iostream>
using namespace std;
const int N=109;
int a[N];
int main() {
    int n;cin>>n;
    for(int i=0;i<=n;++i){
        cin>>a[n];
    }
    int count=0;
    for(int i=0;i<=n;++i){
        count+=a[n];
    }
    cout<<count;
    return 0;
}