const url = "https://min-api.cryptocompare.com/data/pricemulti?fsyms=BTC,ETH&tsyms=USD,EUR";
const url2 = "http://localhost:3000/test"
const url3 = "http://localhost:3000/Search"

function getQueryVariable(variable)
{
       var query = window.location.search.substring(1);
       var vars = query.split("&");
       for (var i=0;i<vars.length;i++) {
               var pair = vars[i].split("=");
               if(pair[0] == variable){return decodeURIComponent(pair[1]);}
       }
       return(false);
}

let vm =new Vue({
    el: '#app',
    data: {
        array: null,
        maxpage : null,
        currentpage : null
    },

    created() {
        axios
        .get(url3, {params: {search: getQueryVariable('search'), ifsearch: true, page: 0}})
        .then(response => {
            this.array = response.data
            this.maxpage = response.data[0].maxPage   
        })

    },
    
    methods: {
        alerttest: function(msg){
            alert(msg)
            alert(getQueryVariable('search'))
        },
        sendmsg: function(){
            if(this.currentpage<1)
                this.currentpage=1
            else if(this.currentpage>this.maxpage)
                this.currentpage = this.maxpage
            
            alert(getQueryVariable('search'))
            axios
            .get(url3, {params: {search: getQueryVariable('search'), ifsearch: false, page: this.currentpage-1}})
            .then(response => {
                this.array = response.data
                this.maxpage = response.data[0].maxPage   
            })
        }
    },
})

/*new Vue({
    el: '#app2',
    data: {
        array: null
    },

    created() {
        axios
        .get(url3, {params: {search: "+英雄聯盟, -Garena", ifsearch: true}})
        .then(response => (this.array = response.data))
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
})*/

Vue.component('news-test', {
    props : ['news'],
    template :  `
        <div class="news">
            <h4 class="row"><a :href= "news.url" class="col-8">{{ news.title }}</a>  <small class="col">score:{{ news.score }}</small></h4>
            <h6><a :href= "news.url" >{{ news.url }}</a></h6>
            <h6>{{ news.content }}</h6>
        </div>
    `
})