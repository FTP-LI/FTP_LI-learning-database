//index.js
Page({
  data: {
    bannerHeight: Math.ceil(249.0 / 800.0 * getApp().screenWidth)
  },
  //事件处理函数
  copyWechat: function() {
    wx.makePhoneCall({
      phoneNumber: '4008226629'
    })
  },
  onLoad: function () {
  },
  onShareAppMessage: function () {
    return {
      title: '泉州大白网络科技',
      desc: '泉州大白网络科技创办于1985年，位于长三角的江苏省常州市，专业生产各种规格的焊割设备及配件。',
      path: '/pages/index/index'
    }
  }
})
