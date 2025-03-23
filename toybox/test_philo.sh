#!/bin/bash

# 哲学者のテストスクリプト
# 使用方法: ./test_philo.sh [実行ファイルへのパス]

# デフォルトのパスを設定
PHILO_PATH=${1:-"./philo"}

# 色の定義
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# エラー処理関数
function error_exit {
    echo -e "${RED}エラー: $1${NC}"
    exit 1
}

# 実行ファイルの存在確認
if [ ! -f "$PHILO_PATH" ]; then
    error_exit "実行ファイル $PHILO_PATH が見つかりません。"
fi

# テスト実行関数
function run_test {
    local test_name=$1
    local args=$2
    local expected_result=$3
    local timeout_seconds=$4
    
    echo -e "\n${BLUE}テスト: $test_name${NC}"
    echo -e "コマンド: $PHILO_PATH $args"
    
    # タイムアウト付きで実行
    timeout $timeout_seconds $PHILO_PATH $args &
    local pid=$!
    
    # 5秒待機
    sleep 5
    
    # プロセスがまだ実行中かチェック
    if ps -p $pid > /dev/null; then
        # 長時間実行テストの場合は成功と見なす
        if [ "$expected_result" == "long_running" ]; then
            echo -e "${GREEN}✓ 予想通り: プログラムは正常に実行中です（$timeout_seconds秒でタイムアウト）${NC}"
            kill $pid 2>/dev/null
        else
            # 死者が出るはずのテストが継続している場合
            echo -e "${RED}✗ 失敗: すべての哲学者が死亡するはずですが、プログラムは実行中です${NC}"
            kill $pid 2>/dev/null
        fi
    else
        # プロセスが終了した場合
        if [ "$expected_result" == "death" ]; then
            echo -e "${GREEN}✓ 予想通り: 哲学者が死亡してプログラムが終了しました${NC}"
        else
            echo -e "${RED}✗ 失敗: プログラムが予期せず終了しました${NC}"
        fi
    fi
}

# バリデーションテスト
echo -e "${YELLOW}===== 引数バリデーションテスト =====${NC}"

echo -e "\n${BLUE}テスト: 引数なし${NC}"
$PHILO_PATH
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ 予想通り: エラーメッセージが表示されました${NC}"
else
    echo -e "${RED}✗ 失敗: エラーメッセージが表示されるべきです${NC}"
fi

echo -e "\n${BLUE}テスト: 引数が不足している${NC}"
$PHILO_PATH 5 800 200
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ 予想通り: エラーメッセージが表示されました${NC}"
else
    echo -e "${RED}✗ 失敗: エラーメッセージが表示されるべきです${NC}"
fi

echo -e "\n${BLUE}テスト: 負の値を含む引数${NC}"
$PHILO_PATH 5 800 200 -100 5
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ 予想通り: エラーメッセージが表示されました${NC}"
else
    echo -e "${RED}✗ 失敗: エラーメッセージが表示されるべきです${NC}"
fi

echo -e "\n${BLUE}テスト: 哲学者数が0${NC}"
$PHILO_PATH 0 800 200 100
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ 予想通り: エラーメッセージが表示されました${NC}"
else
    echo -e "${RED}✗ 失敗: エラーメッセージが表示されるべきです${NC}"
fi

# 基本的なテストケース
echo -e "\n${YELLOW}===== 基本的なテストケース =====${NC}"

# 死亡ケース
run_test "死亡するケース" "1 800 200 200" "death" 10

# 長時間実行ケース
run_test "長時間実行（哲学者が生存し続ける）" "5 800 200 200" "long_running" 10

# 必須回数の食事後に終了するケース
run_test "必須食事回数後に終了" "5 800 200 200 7" "long_running" 30

# 極端なケース
echo -e "\n${YELLOW}===== 極端なケース =====${NC}"

# 極端に短い死亡時間
run_test "極端に短い死亡時間" "5 100 200 200" "death" 10

# 極端に長い食事時間
run_test "極端に長い食事時間" "4 400 300 200" "death" 10

# 多数の哲学者
run_test "多数の哲学者" "20 800 200 200" "long_running" 10

# 特殊なケース
echo -e "\n${YELLOW}===== 特殊なケース =====${NC}"

# デッドロック回避テスト
run_test "デッドロック回避テスト" "2 800 200 200" "long_running" 10

# リソース競合テスト
run_test "リソース競合テスト" "4 410 200 200" "long_running" 10

echo -e "\n${GREEN}テスト完了${NC}"
