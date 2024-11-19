#include <iostream>
using namespace std;
int main() {
    int n;cin>>n;
    int a[n];
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