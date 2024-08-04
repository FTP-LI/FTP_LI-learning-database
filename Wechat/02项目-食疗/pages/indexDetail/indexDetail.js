// pages/indexDetail/indexDetail.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    objInfo:'',//页面的数据容器
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    //options获取从其他页面传递过来的参数变量 
      console.log('indexDetail参数url地址：',options);//{属性：属性值}
      //请求对应的数据信息
    wx.showNavigationBarLoading();
    
      wx.request({
        url: 'http://iwenwiki.com:3002/api/indexlist/detail',
        data:{
          id: options.id
        },
        success:res=>{
          // console.log(res.data[0]);
          //隐藏loading
          wx.hideNavigationBarLoading();

          //动态设置当前页面的标题
          wx.setNavigationBarTitle({
            title: res.data[0].title,
          })
          this.setData({
            objInfo: res.data[0]
          })
        }
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