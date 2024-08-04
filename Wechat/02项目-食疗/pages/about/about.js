// pages/about/about.js
var app=getApp();
Page({

  /**
   * 页面的初始数据
   */
  data: {
    flag:true,//控制授权图形的按钮是否显示
    userInfo:'',//用户信息
    userAvatarUrl:"",//头像
    userNickName:"",//昵称
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    console.log(app.globalData);
    //使用的页面---获取头像
    if(app.globalData.userInfo){//存在
      console.log('全局变量获取')
        this.setData({
          flag:false,
          userAvatarUrl:app.globalData.userInfo.avatarUrl,
          userNickName:app.globalData.userInfo.nickName
        })
    }else{//获取不到全局的变量信息
      //给app.js绑定一个函数---函数返回这个结果
      console.log('进入函数获取用户信息')
      app.userCallback=(res)=>{
        console.log(res);
        this.setData({
          flag:false,
          userAvatarUrl:res.avatarUrl,
          userNickName:res.nickName
        })
      }

    }

  },
  //1.点击按钮---获取用户头像=======================================
  //1.1 点击按钮e对象 获取用户的信息   1.2 当再次进入页面 --会发现头像不存在，必须点击按钮才能存在--（不推荐本地存储---本地存储-保留你的头像 你更换头像 本地不会修改）、
  //1.3 进入小程序 判断之前是否授权头像  如果有授权 获取头像  否则不处理
  getUserInfo(e){
    console.log(e);//判断用户是否允许访问

    if(e.detail.userInfo){
      console.log(e.detail.userInfo);
      //存储全局变量
      app.globalData.userInfo=e.detail.userInfo;
      this.setData({
        flag:false,
        userAvatarUrl:e.detail.userInfo.avatarUrl,
        userNickName:e.detail.userInfo.nickName
      })

    }else{
      console.log('用户拒绝访问');

    }



  },

  //4.授权头像并登录 getUserInfo2
  getUserInfo2(e){
    console.log(e);
    //直接获取用户信息   默认允许获取
    var userInfo=e.detail.userInfo;//用户信息
    this.setData({
      flag:false,
      userAvatarUrl:userInfo.avatarUrl,
      userNickName:userInfo.nickName
    })
    
    //登录---=========================================
    wx.login({
      success: (res) => {
        console.log('调用接口获取登录凭证',res);

        //获取等的凭证code码---发送给后台
        wx.request({
          url: 'http://localhost:3000/getSession',//后台给的接口---code换openid session_key
          data:{
            codeId:res.code
          },
          success:res=>{
            console.log(res.data);
            //存储本地：openid 1.存储后其他页面可以使用 2.再次进入页面的时候获取本地存储openid 直接登录
            wx.setStorageSync('openid', res.data.openid);

            //自定义的登录状态----看后台接口----- 
            // wx.request({
            //   url: 'http://localhost:3000/login',
            //   data:{
            //     openid:res.data.openid,
            //     //用户信息  头像和昵称
            //   },
            //   success:res=>{
            //     console.log('登录的结果',res.data);
            //   }
            // })


          }
        })



      },
    })



  },




  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})