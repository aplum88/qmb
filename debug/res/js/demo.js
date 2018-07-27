
var app = new Vue({
    el: '#app',
    data: {
      userlist:[],
      log:{},
      activeName: 'second',
      options: [],
      value: ''
    
    },
    methods: {
      click_button:function(){
            alert("button click")
      },
      handleClick(tab, event) {
        console.log(tab, event);
      },
      open() {
        this.$alert('这是一段内容', '标题名称', {
          confirmButtonText: '确定',
          callback: action => {
            this.$message({
              type: 'info',
              message: `action: ${ action }`
            });
          }
        });
      },
      open3() {
        this.$prompt('请输入邮箱', '提示', {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          inputPattern: /[\w!#$%&'*+/=?^_`{|}~-]+(?:\.[\w!#$%&'*+/=?^_`{|}~-]+)*@(?:[\w](?:[\w-]*[\w])?\.)+[\w](?:[\w-]*[\w])?/,
          inputErrorMessage: '邮箱格式不正确'
        }).then(({ value }) => {
          this.$message({
            type: 'success',
            message: '你的邮箱是: ' + value
          });
        }).catch(() => {
          this.$message({
            type: 'info',
            message: '取消输入'
          });       
        });
      },
      listuarts(){
         var str = invokeListUarts()
         console.log(str)
         this.options = JSON.parse(str);

        //  var ports = str.split(",")
        //  var self = this
        //  ports.forEach(function(port,i){
        //   　　console.log('forEach遍历:'+i+'--'+port);
        //     var obj = {
        //       value: port,
        //       label: port
        //     }
        //     console.log(self.options)
        //     self.options.push(obj);
        //   })
         //this.options.push({"value":port});
         console.log(this.options)

      }
    
    },
    mounted :function(){
      this.listuarts();
      this.value = this.options[0].value
      //setwindowheight(document.body.offsetHeight);
    }
  });
  