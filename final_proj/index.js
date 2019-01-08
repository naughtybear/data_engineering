var express = require('express')
var app = express()
var exec = require('child_process').exec
var fs = require('fs')

const NEWS_PER_PAGE = 10

function isNAN(obj) {
    //return !(obj === obj);
    if(!obj) return 1;
}

app.use('/view', express.static('view'))

app.get('/', function (req, res){
    res.sendFile(__dirname + '/index.html')
})

app.get('/Search', function(req,res){
    console.log(req.query)
    //console.log(req.query.ifsearch)
    res.header('Access-Control-Allow-Origin', '*');
    let page = parseInt(req.query.page)
    //res.send('just test')
    if(req.query.ifsearch == 'true'){
        console.log("111")
        exec('./dp_search ettoday0.rec -p "'+ req.query.search +'"', (err, stdout) => {
            //if(err) throw err;
            fs.readFile(__dirname + '/out2.txt', (err, data) => {
                //res.send('hahaha')
                if(err) throw err;
                //console.log(typeof(data))
                data = String(data).split('\n')
                let maxPage = Math.ceil((data.length-1) / NEWS_PER_PAGE)
                let array = []
                for(let i=page*NEWS_PER_PAGE; i<Math.min((page+1)*NEWS_PER_PAGE, data.length-1); i++){
                    let buf = data[i].split('\t')
                    let tmp = {
                        "id" : i,
                        "score" : buf[0],
                        "url" : buf[1],
                        "title" : buf[2],
                        "content" : buf[3].substring(0,200),
                        "maxPage" : maxPage
                    }
                    array.push(tmp)
                }
                //res.end ("</table></div>");
                res.send(array)
            })
        })
    }
    else{
        console.log("222")
        fs.readFile(__dirname + '/out2.txt', (err, data) => {
            //res.send('hahaha')
            if(err) throw err;
            //console.log(typeof(data))
            data = String(data).split('\n')
            let maxPage = Math.ceil((data.length-1) / NEWS_PER_PAGE)
            let array = []
            for(let i=page*NEWS_PER_PAGE; i<Math.min((page+1)*NEWS_PER_PAGE, data.length-1); i++){
                let buf = data[i].split('\t')
                let tmp = {
                    "id" : i,
                    "score" : buf[0],
                    "url" : buf[1],
                    "title" : buf[2],
                    "content" : buf[3].substring(0,200),
                    "maxPage" : maxPage
                }
                array.push(tmp)
            }
            //res.end ("</table></div>");
            res.send(array)
        })
    }
})

app.post('/haha', function(req, res){
    res.send("you are idiot")
})

app.get('/test', function(req, res){
    console.log(req.query.id)
    tmp = ["aaa", "bbb", "ccc"]
    res.header('Access-Control-Allow-Origin', '*');
    res.set('Content-Type', 'text/html');
    res.send(req.query.id);
})


app.get('/test2', function (req, res){
    res.sendFile(__dirname + '/test.html')
})

var server = app.listen(3000, function(){
    console.log ('Server跑起來了，現在時間是:' + new Date())
    console.log(__dirname)
})