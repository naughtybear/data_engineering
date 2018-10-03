http = require('http');
url = require('url');
var fs = require('fs');
const {exec}  = require('child_process');


function isNAN(obj) {
    return !(obj === obj);
}

server = http.createServer ((req,res)=>{
   urlData = url.parse(req.url,true);
   action = urlData.pathname;
   res.writeHead (200,{"Content-Type": "text/html"})
   if (action == "/Search"){
       user = urlData.query;
       exec('grep '+user.search + ' ./final.txt',{maxBuffer: 10000 * 60000}, (err, stdout) => {
        if (err) return console.log(err)
        //console.log(stdout)  // VmRSS:     13408 kB
        var content=stdout.split('\n');
        var topic=fs.readFileSync('topic.txt','utf-8');
        topic=topic.split('\n');
        res.write('<head><meta charset="utf-8"/></head>');
        res.write("<div> <table  width=1000 border=1 >");
        console.log(topic.length);
        for(var i=0;i<content.length;i++){
            //res.write("<div> <table class=\"table table-striped\">");
            var buf=content[i].substring(0,content[i].length-7);
            var num=content[i].substring(content[i].length-7)
            //console.log(num);
            num=parseInt(num);
            if(isNAN(num))
                continue;
            console.log(topic[2*num]);
            res.write("<tr>");
            res.write("<td><a href="+topic[2*num]+">"+topic[2*num+1]+"</a></td>");
            res.write("<td>"+buf+"</td>");
            res.write("</tr>");
        }
        res.end ("</table></div>");
       })
   }
});

   
server.listen(3000);

console.log ('Server跑起來了，現在時間是:' + new Date());