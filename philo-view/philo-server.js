const express = require('express');
const path = require('path');
const { spawn } = require('child_process');
const app = express();
const port = 3000;

// 静的ファイルの提供
app.use(express.static(__dirname));
app.use(express.json());

// メインページにアクセスした時の処理
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'index.html'));
});

// 実際の哲学者プログラムをAPI経由で実行する機能
app.post('/api/run-philo', (req, res) => {
    const { nbPhilo, timeToDie, timeToEat, timeToSleep, mustEatCount } = req.body;
    
    // コマンドライン引数を構築
    const args = [nbPhilo, timeToDie, timeToEat, timeToSleep];
    if (mustEatCount > 0) {
        args.push(mustEatCount);
    }
    
    // ../mandatory/philo を実行
    const philoProcess = spawn('../mandatory/philo', args);
    
    let output = '';
    let errorOutput = '';
    
    philoProcess.stdout.on('data', (data) => {
        output += data.toString();
    });
    
    philoProcess.stderr.on('data', (data) => {
        errorOutput += data.toString();
    });
    
    philoProcess.on('close', (code) => {
        res.json({
            success: code === 0,
            output,
            error: errorOutput,
            exitCode: code
        });
    });
});

// サーバー起動
app.listen(port, () => {
    console.log(`running on http://localhost:${port}`);
});
