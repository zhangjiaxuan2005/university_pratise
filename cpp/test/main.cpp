#include <bits/stdc++.h>
using namespace std;

int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0); cout.tie(0);
    int n, k;
    cin >> n >> k;

    vector<int> nums(n);
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }

    vector<int> dp(k + 1, 0);
    int maxGCD = 0;

    for (int i = 0; i < n; ++i) {
        vector<int> newDp(k + 1, 0);
        for (int j = 0; j <= k; ++j) {
            newDp[j] = dp[j];
            if (j > 0 && nums[i] % dp[j - 1] == 0) {
                newDp[j] = max(newDp[j], gcd(nums[i], dp[j - 1]));
            }
            maxGCD = max(maxGCD, newDp[j]);
        }
        dp = newDp;
    }

    // 输出调试信息
    cout << "Final maxGCD: " << maxGCD << endl;

    return 0;
}
