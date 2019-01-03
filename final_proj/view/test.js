const url = "https://min-api.cryptocompare.com/data/pricemulti?fsyms=BTC,ETH&tsyms=USD,EUR";
const url2 = "http://localhost:3000/test"
const url3 = "http://localhost:3000/Search"

new Vue({
    el: '#app',
    data: {
        array: []
    },

    created() {
        axios
        .get(url3, {params: {search: "+英雄聯盟, -Garena"}})
        .then(response => (this.array = response.data))
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
})