// pages/index/index.js
var http=require('../../utils/http.js');
Page({

  /**
   * 页面的初始数据
   */
  data: {
    current:1,
    bannerArr:[],//轮播数据
    list:[],//列表信息容器
  },
//1.swiper轮播的时候触发事件 swiperChange
  swiperChange:function(e){
    // console.log(e.detail);//获取当前的轮播的位置的current  修改data里面的数据 current 页面数据同步修改
    this.setData({
      current: e.detail.current+1
    })
  },
  onLoad: function (options) {
    //2.进入页面获取轮播图接口数据  api  发起 HTTPS 网络请求
    console.log(this);
    var that=this;
    //获取动态数据-----模块http.js
    console.log(http);
    //参数http(method,params,url,message,callback,error)
    http('get','','/api/banner','加载中',(res)=>{
      console.log(res);
         this.setData({
              bannerArr: res.data
          })
    });


    // wx.request({
    //   method:'get',
    //   url: 'http://iwenwiki.com:3002/api/banner',
    //   success:function(res){
    //     console.log(res.data);
    //     console.log(res.data.data);
    //     // console.log(this);//undefined
    //     that.setData({
    //       bannerArr: res.data.data
    //     })
    //   }
    // })

    //------es6箭头函数-------------------------------
    // wx.request({
    //   method: 'get',
    //   url: 'http://iwenwiki.com:3002/api/banner',
    //   success: res =>{ //function (res){}   箭头函数：  res=>{}
    //     console.log(res.data);
    //     console.log(res.data.data);
    //     this.setData({
    //       bannerArr: res.data.data
    //     })
    //   }
    // })

    ////////==============================================
    http('get','','/api/indexlist','数据拼命加载中',(res)=>{
       console.log('信息列表',res);
        this.setData({
              list:res.data
            })
    });

    //显示loading加载数据
    // wx.showLoading({
    //   title: '数据拼命加载中',
    // })

  //3.获取列表信息数据
    // wx.request({
    //   method: 'get',
    //   url: 'http://iwenwiki.com:3002/api/indexlist',
    //   success:res=>{
    //     console.log(res.data)
    //     //加载完毕--提示框信息--1.5s 自动隐藏
    //     wx.showToast({
    //       // icon:'none',
    //       title: '数据加载完毕',
    //     })

    //     this.setData({
    //       list:res.data.data
    //     })
    //   },
    //   complete:function(){
    //     //隐藏loading
    //     wx.hideLoading();
    //   }

    // })

  },
  //4.点击首页的list信息的列表---跳转到详情页面-----------------
  tiao: function (e) {
    //跳转页面---小程序的api 
    console.log(e.currentTarget.dataset.mark);
    //e.currentTarget.dataset.mark
    wx.navigateTo({
      url: '../indexDetail/indexDetail?id=' + e.currentTarget.dataset.mark,
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