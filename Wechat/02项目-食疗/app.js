//app.js
App({

  /**
   * 当小程序初始化完成时，会触发 onLaunch（全局只触发一次）
   */
  onLaunch: function () {

    //2.再次进入页面---之前已经登录授权过---进入小程序--自动登录
    //2.1 获取本地有没有你存储的登录标识  openid 
    var openid=wx.getStorageSync('openid');
    if(openid){ //存在--之前授权登录
      //直接登录
      wx.request({
        url: 'http://localhost:3000/login',
        data:{
          openid:openid
        },
        success:res=>{
          console.log(res.data);

        }
      })

    }else{
      console.log('之前没有授权登录---引导用户--进行授权登录')
      //弹框提示--去登录---授权
      // wx.switchTab({
      //   url: '/pages/about/about',
      // })

    }




    //1.进入小程序---获取之前是否授权头像=========================
    //获取用户的当前设置。返回值中只会出现小程序已经向用户请求过的权限
    wx.getSetting({
      success: (res) => {
        console.log(res.authSetting);
        //判断是否授权头像
        if(res.authSetting['scope.userInfo']){
          console.log('之前已经授权头像');

          //获取小程序的头像 api调用、
          wx.getUserInfo({
            success: (res) => {
              console.log('获取用户信息 ',res.userInfo);
              //给全局变量
              this.globalData.userInfo=res.userInfo;

              //获取的api方法都是异步的，直接进入小程序--立马获取头像昵称 因为执行api是异步的，获取不到全局变量----如果进入首页--然后其他页面获取用户头像--这样全局存储是可以的
              //进入小程序---当前的页面立马使用头像昵称---异步---函数形式返回
              if(this.userCallback){
                  this.userCallback(res.userInfo);
              }

            },
          })
        }else{

          console.log('之前没有授权头像')
        }

      },
    })

  },

  
  globalData:{
    cityName:'',//切换城市名字
    userInfo:null,//用户头像
  }
})
