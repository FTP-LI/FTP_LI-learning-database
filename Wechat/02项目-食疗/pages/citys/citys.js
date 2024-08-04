// pages/citys/citys.js
var app=getApp();

Page({
  data: {
    hotCitys:[],//热门城市
  },
  //2.点击定位按钮---获取当前的位置---
  getLocation:function(){
    // console.log(app);

      wx.getLocation({
        success: function(res) {
          console.log(res);//longitude   latitude
          //http://iwenwiki.com:3002/api/lbs/location?latitude=39.90&longitude=116.4
          wx.request({
            url: 'http://iwenwiki.com:3002/api/lbs/location',
            data:{
              latitude: res.latitude,
              longitude: res.longitude
            },
            success:result=>{
              console.log(result.data.result);
              console.log(result.data.result.ad_info.city);
              //获取当前的位置的城市 名字---返回到食疗坊界面--切换城市 
              var citys = result.data.result.ad_info.city;
              var cityName = citys.slice(0, citys.length-1);
              //跳转---食疗坊界面--把数据传递过去--- url 
              // wx.switchTab({//可以跳转页面---但是不能传递url值
              //   url: '../food/food',
              // })

              //1.----url地址传递参数：-------------------------
              // wx.reLaunch({
              //   url: '../food/food?cityName=' + cityName,
              // })

              //2.-把获取的变量存储给全局app.js里面的全局变量 globalData存储
              // app.globalData.cityName = cityName;
              // wx.switchTab({
              //   url: '../food/food',
              // })
              
              //3.本地存储-----
              wx.setStorageSync('cityName', cityName);
              wx.switchTab({
                url: '../food/food',
              })  
            }
          })
        },
      })
  },
  onLoad: function (options) {
    //1.获取热门城市
    wx.request({
      url: 'http://iwenwiki.com:3002/api/hot/city',
      success:res=>{
        console.log(res.data)
        this.setData({
          hotCitys:res.data.data
        })
      }
    })
  },
  //3.切换热门城市-------1.点击获取热门城市  2.跳转页面传递热门城市----------------
  selectCity:function(e){
    var citys=e.currentTarget.dataset.val;
    // console.log(citys);
    wx.setStorageSync('cityName', citys);
    wx.switchTab({
      url: '../food/food',
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