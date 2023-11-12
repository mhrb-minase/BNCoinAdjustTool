#include<iostream>
#include<vector>
#include<string>
#include<functional>
#include<algorithm>

constexpr int coinLimit = 500000;  // 扱えるコイン数の上限
constexpr int inf = (1 << 30) - 1; // 最小化したい変数の初期値

struct Purchase
{
    int coin;
    int yen;
};

struct State
{
    int operationCount;
    int price;
};

// 購入できるコイン数と金額の定義
std::vector<Purchase> purchases = {
    { coin: 300, yen: 330 },
    { coin: 500, yen: 550 },
    { coin: 1000, yen: 1100 },
    { coin: 2000, yen: 2200 },
    { coin: 3000, yen: 3300 },
    { coin: 5000, yen: 5500 },
    { coin: 10000, yen: 11000 },
    { coin: 273, yen: 300 },
    { coin: 455, yen: 500 },
    { coin: 910, yen: 1000 },
    { coin: 1819, yen: 2000 },
    { coin: 2728, yen: 3000 },
    { coin: 4546, yen: 5000 },
    { coin: 9091, yen: 10000 }
};

int input_non_negative_integer(std::string name, int upperLimit)
{
    int input = -1;
    while (true) {
        std::cout << " input " + name + "> ";
        std::cin >> input;
        if (0 <= input && input <= upperLimit) {
            break;
        } else {
            std::cout << " input value between 0 and " + std::to_string(upperLimit) << std::endl;
        }
    }
    return input;
}

// 計算関数
// compには(回数, 金額)の組に対する<演算子に対応する比較関数を入れる
// 購入操作の回数が入ったvectorが返る
std::vector<int> calculate(int targetCoin, int currentCoin, std::function<bool(State, State)> comp)
{
    int remainCoin = targetCoin - currentCoin;  // 残りコイン数=購入すべきコイン数

    // ぴったり買えない場合が存在するので、計算時に余分にコインを買う場合も考慮する
    int virtualRemainCoin = remainCoin;
    for (Purchase p : purchases) {
        virtualRemainCoin = std::max(virtualRemainCoin, remainCoin + p.coin);
    }

    // 比較関数に従って最適化したときの(回数, 金額)の組
    std::vector<State> dp(virtualRemainCoin + 1, { operationCount: inf, price: inf });
    dp[0] = { operationCount: 0, price: 0 };

    // 直前の操作 基本的にpurchasesに対応するが、-1で1コイン上から来たことを示す
    std::vector<int> prev(virtualRemainCoin + 1, -2);

    for (int i = 0; i < virtualRemainCoin; ++i) {
        for (int j = 0; j < (int)purchases.size(); ++j) {
            Purchase p = purchases[j];

            // 必要以上に購入するケースを弾く
            if (i + p.coin > virtualRemainCoin) {
                continue;
            }

            State currentState = dp[i];
            State nextState = { operationCount: currentState.operationCount + 1, price: currentState.price + p.yen };
            if (comp(nextState, dp[i + p.coin])) {
                dp[i + p.coin] = nextState;
                prev[i + p.coin] = j;
            }
        }
    }

    // 余分に買った場合を考慮する
    for (int i = virtualRemainCoin; i > 0; --i) {
        if (comp(dp[i], dp[i - 1])) {
            dp[i - 1] = dp[i];
            prev[i - 1] = -1;
        }
    }

    std::vector<int> operationCount(purchases.size(), 0);  // 各操作を何回したか

    // 必要なコイン数から遡る
    int coin = remainCoin;
    while (coin != 0) {
        if (prev[coin] == -1) {
            ++coin;
        } else {
            ++operationCount[prev[coin]];
            coin -= purchases[prev[coin]].coin;
        }
    }

    return operationCount;
}

void output(std::string patternName, std::vector<int>& operationCount)
{
    int totalCoins = 0;  // 買うコイン数の合計
    int totalPrice = 0;  // 合計金額

    std::cout << "\n " << patternName << '\n';

    for (int i = 0; i < (int)operationCount.size(); ++i) {
        int count = operationCount[i];

        int coin = purchases[i].coin;
        int yen = purchases[i].yen;

        totalCoins += coin * count;
        totalPrice += yen * count;

        if (count == 0) {
            continue;
        }

        std::cout << ' ' << coin << " coins  " << yen << " yen  " << count << " times\n";
    }

    std::cout << "\n total  " << totalCoins << " coins   " << totalPrice << " yen" << std::endl;

    return;
}

int main()
{
    int targetCoin = 0;  // 目標コイン数
    int currentCoin = 0;  // 所持コイン数

    // 入力
    targetCoin = input_non_negative_integer("target coin", coinLimit);
    currentCoin = input_non_negative_integer("current coin", targetCoin);

    // 金額を最小化した場合と回数を最小化した場合の計算
    auto priceComp = [](State l, State r) {
        return l.price == r.price ? l.operationCount < r.operationCount : l.price < r.price;
    };
    std::vector<int> priceMinimizeOperationCount = calculate(targetCoin, currentCoin, priceComp);

    auto countComp = [](State l, State r) {
        return l.operationCount == r.operationCount ? l.price < r.price : l.operationCount < r.operationCount;
    };
    std::vector<int> countMinimizeOperationCount = calculate(targetCoin, currentCoin, countComp);

    // 出力
    output("minimize price", priceMinimizeOperationCount);
    output("minimize operation count", countMinimizeOperationCount);

    return 0;
}