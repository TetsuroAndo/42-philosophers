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
    
    // philoを実行
    const philoPath = path.join(__dirname, '../mandatory/philo');
    console.log('Executing philo at:', philoPath, 'with args:', args);
    const philoProcess = spawn(philoPath, args);
    console.log('Philo process started');
    
    let output = '';
    let errorOutput = '';
    
    philoProcess.stdout.on('data', (data) => {
        const str = data.toString();
        console.log(str);
        output += str;
    });
    
    philoProcess.stderr.on('data', (data) => {
        const str = data.toString();
        console.error(str);
        errorOutput += str;
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
