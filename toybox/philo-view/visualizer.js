// 状態の定義
const STATE = {
    THINKING: 'thinking',
    EATING: 'eating',
    SLEEPING: 'sleeping',
    DEAD: 'dead',
    HAS_FORK: 'has-fork'
};

// フォークの状態
const FORK_STATE = {
    ON_TABLE: 'on-table',
    PICKED_UP: 'picked-up'
};

// ビジュアライザークラス
class PhilosopherVisualizer {
    constructor() {
        this.philosophers = [];
        this.forks = [];
        this.running = false;
        this.startTime = 0;
        this.elapsedTimeInterval = null;
        this.simulationInterval = null;
        this.timelineInterval = null;

        this.tableContainer = document.getElementById('table-container');
        this.table = document.getElementById('table');
        this.timeDisplay = document.getElementById('elapsed-time');
        this.logContainer = document.getElementById('log-container');
        this.timelineRows = document.getElementById('timeline-rows');
        this.timelineTimestamps = document.getElementById('timeline-timestamps');
        
        // タイムラインの状態履歴を保持
        this.timelineHistory = new Map();

        // 設定要素
        this.philoCountInput = document.getElementById('philo-count');
        this.timeToDieInput = document.getElementById('time-to-die');
        this.timeToEatInput = document.getElementById('time-to-eat');
        this.timeToSleepInput = document.getElementById('time-to-sleep');
        this.mustEatCountInput = document.getElementById('must-eat-count');

        // ボタン
        this.startBtn = document.getElementById('start-btn');
        this.stopBtn = document.getElementById('stop-btn');
        this.resetBtn = document.getElementById('reset-btn');

        // イベントリスナーの設定
        this.startBtn.addEventListener('click', () => this.start());
        this.stopBtn.addEventListener('click', () => this.stop());
        this.resetBtn.addEventListener('click', () => this.reset());
    }

    // シミュレーション開始
    start() {
        if (this.running) return;

        const philoCount = parseInt(this.philoCountInput.value);
        const timeToDie = parseInt(this.timeToDieInput.value);
        const timeToEat = parseInt(this.timeToEatInput.value);
        const timeToSleep = parseInt(this.timeToSleepInput.value);
        const mustEatCount = parseInt(this.mustEatCountInput.value);

        if (philoCount < 1) {
            alert('少なくとも1人の哲学者が必要です');
            return;
        }

        this.running = true;
        this.startTime = Date.now();
        this.startBtn.disabled = true;
        this.stopBtn.disabled = false;
        
        // 哲学者とフォークを生成
        this.createPhilosophers(philoCount);
        this.createForks(philoCount);
        
        // ログとタイムラインを初期化
        this.logContainer.innerHTML = '';
        this.addLogEntry('シミュレーション開始', 'general');
        this.addLogEntry('青: 考え中 / 緑: 食事中 / 紫: 睡眠中 / 赤: 死亡', 'general');
        this.initializeTimeline(philoCount);
        
        // 経過時間の更新
        this.elapsedTimeInterval = setInterval(() => {
            const elapsedTime = Date.now() - this.startTime;
            this.timeDisplay.textContent = elapsedTime;
        }, 10);
        
        // シミュレーション実行
        this.runSimulation(philoCount, timeToDie, timeToEat, timeToSleep, mustEatCount);
    }

    // シミュレーション停止
    stop() {
        this.running = false;
        this.startBtn.disabled = false;
        this.stopBtn.disabled = true;
        clearInterval(this.elapsedTimeInterval);
        clearInterval(this.simulationInterval);
        clearInterval(this.timelineInterval);
        this.addLogEntry('シミュレーション停止', 'general');
    }

    // リセット
    reset() {
        this.stop();
        this.philosophers = [];
        this.forks = [];
        this.tableContainer.querySelectorAll('.philosopher, .fork').forEach(el => el.remove());
        this.timeDisplay.textContent = '0';
        this.logContainer.innerHTML = '';
        this.addLogEntry('シミュレーションをリセットしました', 'general');
    }

    // 哲学者の作成
    createPhilosophers(count) {
        const tableRadius = this.table.offsetWidth / 2;
        const philoRadius = 25; // 哲学者の円の半径
        const distanceFromCenter = tableRadius * 0.8;
        
        // 既存の哲学者を削除
        this.tableContainer.querySelectorAll('.philosopher').forEach(el => el.remove());
        this.philosophers = [];
        
        for (let i = 0; i < count; i++) {
            // 円周上の位置を計算
            const angle = (2 * Math.PI * i) / count;
            const x = Math.cos(angle) * distanceFromCenter;
            const y = Math.sin(angle) * distanceFromCenter;
            
            // 哲学者のDOM要素を作成
            const philoElement = document.createElement('div');
            philoElement.className = 'philosopher thinking';
            philoElement.id = `philo-${i+1}`;
            philoElement.textContent = i + 1;
            
            // 左右の手
            const leftHand = document.createElement('div');
            leftHand.className = 'left-hand';
            
            const rightHand = document.createElement('div');
            rightHand.className = 'right-hand';
            
            philoElement.appendChild(leftHand);
            philoElement.appendChild(rightHand);
            
            // 位置を設定
            philoElement.style.left = `calc(50% + ${x}px - ${philoRadius}px)`;
            philoElement.style.top = `calc(50% + ${y}px - ${philoRadius}px)`;
            
            this.tableContainer.appendChild(philoElement);
            
            // 状態オブジェクトを作成
            this.philosophers.push({
                id: i + 1,
                element: philoElement,
                state: STATE.THINKING,
                eatCount: 0,
                lastEatTime: 0,
                leftFork: i,
                rightFork: (i + 1) % count,
                hasLeftFork: false,
                hasRightFork: false
            });
        }
    }

    // フォークの作成
    createForks(count) {
        const tableRadius = this.table.offsetWidth / 2;
        const distanceFromCenter = tableRadius * 0.55;
        
        // 既存のフォークを削除
        this.tableContainer.querySelectorAll('.fork').forEach(el => el.remove());
        this.forks = [];
        
        for (let i = 0; i < count; i++) {
            // 円周上の位置を計算（哲学者の間）
            const angle = (2 * Math.PI * i) / count + (Math.PI / count);
            const x = Math.cos(angle) * distanceFromCenter;
            const y = Math.sin(angle) * distanceFromCenter;
            
            // フォークのDOM要素を作成
            const forkElement = document.createElement('div');
            forkElement.className = 'fork';
            forkElement.id = `fork-${i}`;
            
            // 位置を設定
            forkElement.style.left = `calc(50% + ${x}px)`;
            forkElement.style.top = `calc(50% + ${y}px)`;
            forkElement.style.transform = `rotate(${angle + Math.PI/2}rad)`;
            
            this.tableContainer.appendChild(forkElement);
            
            // 状態オブジェクトを作成
            this.forks.push({
                id: i,
                element: forkElement,
                state: FORK_STATE.ON_TABLE,
                ownerId: null
            });
        }
    }

    // シミュレーション実行
    runSimulation(philoCount, timeToDie, timeToEat, timeToSleep, mustEatCount) {
        // シミュレーション開始時間
        const simStartTime = Date.now();
        let allFinished = false;

        // シミュレーションの更新間隔
        this.simulationInterval = setInterval(() => {
            if (!this.running) return;
            
            const currentTime = Date.now();
            const elapsedTime = currentTime - simStartTime;
            
            // すべての哲学者が規定回数食べたかチェック
            if (mustEatCount > 0) {
                allFinished = this.philosophers.every(p => p.eatCount >= mustEatCount);
                if (allFinished) {
                    this.addLogEntry('すべての哲学者が規定回数食べました', 'general');
                    this.stop();
                    return;
                }
            }
            
            // 全員死亡しているかチェック
            const allDead = this.philosophers.every(p => p.state === STATE.DEAD);
            if (allDead) {
                this.addLogEntry('全ての哲学者が死亡しました', 'dead');
                this.stop();
                return;
            }

            // 各哲学者の状態を更新
            this.philosophers.forEach((philo, index) => {
                // 死亡していたらスキップ
                if (philo.state === STATE.DEAD) return;
                
                // 最後に食べてからの経過時間
                const timeSinceLastEat = currentTime - philo.lastEatTime;
                
                // 死亡チェック
                if (philo.lastEatTime > 0 && timeSinceLastEat > timeToDie) {
                    this.changePhilosopherState(philo, STATE.DEAD);
                    this.addLogEntry(`${elapsedTime} ${philo.id} 死亡`, 'dead');
                    return;
                }
                
                // 状態に応じたアクション
                const random = Math.random(); // ランダム要素を加えて動作をバラけさせる
                
                switch (philo.state) {
                    case STATE.THINKING:
                        // フォークを取ろうとする
                        if (random < 0.3) { // 33%の確率で行動
                            this.tryTakeForks(philo, elapsedTime);
                        }
                        break;
                        
                    case STATE.EATING:
                        // 規定の時間食べたらフォークを置く
                        if (elapsedTime - philo.stateStartTime >= timeToEat) {
                            this.dropForks(philo);
                            this.changePhilosopherState(philo, STATE.SLEEPING);
                            philo.stateStartTime = elapsedTime;
                            this.addLogEntry(`${elapsedTime} ${philo.id} 睡眠中`, 'sleeping');
                        }
                        break;
                        
                    case STATE.SLEEPING:
                        // 規定の時間寝たら考え始める
                        if (elapsedTime - philo.stateStartTime >= timeToSleep) {
                            this.changePhilosopherState(philo, STATE.THINKING);
                            this.addLogEntry(`${elapsedTime} ${philo.id} 考え中`, 'thinking');
                        }
                        break;
                }
            });
            
        }, 50); // 50msごとに更新
    }

    // フォークを取ろうとする
    tryTakeForks(philo, elapsedTime) {
        const leftFork = this.forks[philo.leftFork];
        const rightFork = this.forks[philo.rightFork];
        
        // 既に両方のフォークを持っている場合は何もしない
        if (philo.hasLeftFork && philo.hasRightFork) return;
        
        // 偶数IDの哲学者は右→左、奇数IDは左→右の順でフォークを取る
        const isEven = philo.id % 2 === 0;
        
        if (isEven) {
            // 右フォーク→左フォーク
            if (!philo.hasRightFork && rightFork.state === FORK_STATE.ON_TABLE) {
                this.takeFork(philo, rightFork, 'right', elapsedTime);
            } else if (philo.hasRightFork && !philo.hasLeftFork && leftFork.state === FORK_STATE.ON_TABLE) {
                this.takeFork(philo, leftFork, 'left', elapsedTime);
                
                // 両方のフォークを持ったら食べ始める
                if (philo.hasRightFork && philo.hasLeftFork) {
                    this.startEating(philo, elapsedTime);
                }
            }
        } else {
            // 左フォーク→右フォーク
            if (!philo.hasLeftFork && leftFork.state === FORK_STATE.ON_TABLE) {
                this.takeFork(philo, leftFork, 'left', elapsedTime);
            } else if (philo.hasLeftFork && !philo.hasRightFork && rightFork.state === FORK_STATE.ON_TABLE) {
                this.takeFork(philo, rightFork, 'right', elapsedTime);
                
                // 両方のフォークを持ったら食べ始める
                if (philo.hasRightFork && philo.hasLeftFork) {
                    this.startEating(philo, elapsedTime);
                }
            }
        }
    }

    // フォークを取る
    takeFork(philo, fork, side, elapsedTime) {
        fork.state = FORK_STATE.PICKED_UP;
        fork.ownerId = philo.id;
        
        // フォークの位置を哲学者の位置に移動
        const philoRect = philo.element.getBoundingClientRect();
        const tableRect = this.table.getBoundingClientRect();
        
        if (side === 'left') {
            philo.hasLeftFork = true;
            philo.element.classList.add('has-left-fork');
            fork.element.style.left = `${philoRect.left - tableRect.left + philoRect.width / 4}px`;
            fork.element.style.top = `${philoRect.top - tableRect.top + philoRect.height / 2}px`;
            fork.element.style.transform = 'rotate(45deg)';
        } else {
            philo.hasRightFork = true;
            philo.element.classList.add('has-right-fork');
            fork.element.style.left = `${philoRect.right - tableRect.left - philoRect.width / 4}px`;
            fork.element.style.top = `${philoRect.top - tableRect.top + philoRect.height / 2}px`;
            fork.element.style.transform = 'rotate(-45deg)';
        }
        
        this.addLogEntry(`${elapsedTime} 哲学者${philo.id} → F${fork.id}`, 'fork');
    }

    // 食事開始
    startEating(philo, elapsedTime) {
        this.changePhilosopherState(philo, STATE.EATING);
        philo.stateStartTime = elapsedTime;
        philo.lastEatTime = Date.now();
        philo.eatCount++;
        this.addLogEntry(`${elapsedTime} ${philo.id} 食事中 (${philo.eatCount}回目)`, 'eating');
    }

    // フォークを置く
    dropForks(philo) {
        // 左フォーク
        if (philo.hasLeftFork) {
            const leftFork = this.forks[philo.leftFork];
            leftFork.state = FORK_STATE.ON_TABLE;
            leftFork.ownerId = null;
            this.resetForkPosition(leftFork.id);
            philo.hasLeftFork = false;
            philo.element.classList.remove('has-left-fork');
        }
        
        // 右フォーク
        if (philo.hasRightFork) {
            const rightFork = this.forks[philo.rightFork];
            rightFork.state = FORK_STATE.ON_TABLE;
            rightFork.ownerId = null;
            this.resetForkPosition(rightFork.id);
            philo.hasRightFork = false;
            philo.element.classList.remove('has-right-fork');
        }
    }

    // フォークを元の位置に戻す
    resetForkPosition(forkId) {
        const tableRadius = this.table.offsetWidth / 2;
        const distanceFromCenter = tableRadius * 0.55;
        const count = this.forks.length;
        
        const angle = (2 * Math.PI * forkId) / count + (Math.PI / count);
        const x = Math.cos(angle) * distanceFromCenter;
        const y = Math.sin(angle) * distanceFromCenter;
        
        const forkElement = this.forks[forkId].element;
        forkElement.style.left = `calc(50% + ${x}px)`;
        forkElement.style.top = `calc(50% + ${y}px)`;
        forkElement.style.transform = `rotate(${angle + Math.PI/2}rad)`;
    }

    // 哲学者の状態を変更
    changePhilosopherState(philo, newState) {
        philo.element.classList.remove(philo.state);
        philo.element.classList.add(newState);
        philo.state = newState;
    }

    // ログエントリーを追加
    addLogEntry(message, type) {
        const entry = document.createElement('div');
        entry.className = `log-entry ${type}`;
        
        // ログのフォーマットを整える
        if (message.includes('シミュレーション') || message.includes('general')) {
            // システムメッセージ
            entry.textContent = message;
        } else {
            // タイムスタンプ付きメッセージのフォーマットを整える
            const parts = message.split(' ');
            if (parts.length >= 3) {
                const timestamp = parseInt(parts[0]);
                const philoId = parseInt(parts[1]);
                const status = parts.slice(2).join(' ');
                
                entry.innerHTML = `<span class="timestamp">${timestamp}ms</span> <span class="philo-id">哲学者 ${philoId}</span>: ${status}`;
                
                // タイムラインに状態を追加
                if (type !== 'fork') {
                    this.updateTimelineEvent(philoId, timestamp, type);
                }
            } else {
                entry.textContent = message;
            }
        }
        
        this.logContainer.appendChild(entry);
        this.logContainer.scrollTop = this.logContainer.scrollHeight;
    }

    // タイムラインの初期化
    initializeTimeline(philoCount) {
        this.timelineHistory.clear();
        this.timelineRows.innerHTML = '';
        this.timelineTimestamps.innerHTML = '';
        
        // タイムラインの行を作成
        for (let i = 1; i <= philoCount; i++) {
            const row = document.createElement('div');
            row.className = 'timeline-row';
            row.innerHTML = `
                <div class="timeline-label">哲学者${i}</div>
                <div class="timeline-events" id="timeline-events-${i}"></div>
            `;
            this.timelineRows.appendChild(row);
            
            // 各哲学者の状態履歴を初期化
            this.timelineHistory.set(i, [{
                startTime: 0,
                type: 'thinking'
            }]);
        }
        
        // タイムラインの更新を開始
        this.updateTimeline();
    }

    // タイムラインのイベントを更新
    updateTimelineEvent(philoId, timestamp, type) {
        const history = this.timelineHistory.get(philoId) || [];
        history.push({
            startTime: timestamp,
            type: type
        });
        this.timelineHistory.set(philoId, history);
    }

    // タイムラインの表示を更新
    updateTimeline() {
        if (this.timelineInterval) {
            clearInterval(this.timelineInterval);
        }
        
        this.timelineInterval = setInterval(() => {
            if (!this.running) return;
            
            const currentTime = Date.now() - this.startTime;
            const timeScale = 100; // 100ピクセルあたり1000ms
            
            // 各哲学者のタイムラインを更新
            this.timelineHistory.forEach((history, philoId) => {
                const eventsContainer = document.getElementById(`timeline-events-${philoId}`);
                eventsContainer.innerHTML = '';
                
                history.forEach((event, index) => {
                    const nextEvent = history[index + 1];
                    const duration = nextEvent ? nextEvent.startTime - event.startTime : currentTime - event.startTime;
                    
                    const eventElement = document.createElement('div');
                    eventElement.className = `timeline-event ${event.type}`;
                    eventElement.style.left = `${event.startTime / 10}px`;
                    eventElement.style.width = `${duration / 10}px`;
                    
                    eventsContainer.appendChild(eventElement);
                });
            });
        }, 50);
    }
}

// ページロード時にビジュアライザーを初期化
document.addEventListener('DOMContentLoaded', () => {
    const visualizer = new PhilosopherVisualizer();
});
