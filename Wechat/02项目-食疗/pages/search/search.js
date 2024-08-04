// pages/search/search.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    list:[],
  },
  //1.获取表单input输入的内容
  searchInput:function(e){
    // console.log('输入的内容',e.detail);

    //当输入内容才发送请求  
    if(e.detail.value){
        //请求数据 http://iwenwiki.com:3002/api/foods/select?name=产品名字city=城市名
        wx.request({
          url: 'http://iwenwiki.com:3002/api/foods/select',
          data:{
            name:e.detail.value,
            city:''
          },
          success:res=>{
            console.log(res.data);
            if(res.data.status==200){
                this.setData({
                  list:res.data.data
                })
            }else{
              console.log('查无数据')
            }
          }
        })
    }else{
      console.log('输入内容不能为空');
      //清空数据
      this.setData({
        list:[]
      })
    }

  },
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {

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