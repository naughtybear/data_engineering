var express = require('express')
var app = express()
var exec = require('child_process').exec
var fs = require('fs')

function isNAN(obj) {
    //return !(obj === obj);
    if(!obj) return 1;
}

app.get('/', function (req, res){
    res.sendFile(__dirname + '/index.html')
})

app.get('/Search', function(req,res){
    console.log(req.query.search)
    //res.send('just test')
    exec('./dp_search ettoday0.rec -p "'+ req.query.search +'"', (err, stdout) => {
        //if(err) throw err;
        fs.readFile(__dirname + '/out2.txt', (err, data) => {
            //res.send('hahaha')
            if(err) throw err;
            //console.log(typeof(data))
            data = String(data).split('\n')
            res.write('<head><meta charset="utf-8"/></head>');
            res.write('<form action="http://127.0.0.1:3000/Search" method="GET">Search: <input type="text" name="search"> <input type="submit" value="Submit"></form>')
            res.write('<div> <table  width=1000 border=1 >');
            for(let i=0; i<data.length; i++){
                let buf = data[i].split('\t')
                if(isNAN(buf[1]))
                    continue;
                res.write('<tr>')
                res.write('<td><a href='+buf[1]+'>'+buf[2]+'</a></td>')
                res.write('<td>' + buf[0] + '</td>') 
                res.write('<td>' + buf[3] + '</td>') 
            }
            res.end ("</table></div>");
        })
        //res.send(stdout)
    })
})

app.post('/haha', function(req, res){
    res.send("you are idiot")
})


var server = app.listen(3000, function(){
    console.log ('Server跑起來了，現在時間是:' + new Date())
    console.log(__dirname)
})