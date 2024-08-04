// pages/food/food.js
var app=getApp();
Page({

  /**
   * 页面的初始数据
   */
  data: {
    location:'上海',
    num:1,//当前的页面数据
    listArr:[],//列表信息
    msg:'',//页面加载更多数据提示信息
    navBarArr:[
      {
        id:0,
        imgUrl:'../../images/item1.jpg',
        name:'美容养颜'
      },
      {
        id: 1,
        imgUrl: '../../images/item2.jpg',
        name: '保健调养'
      },
      {
        id: 2,
        imgUrl: '../../images/item3.jpg',
        name: '补养'
      },
      {
        id: 3,
        imgUrl: '../../images/item4.jpg',
        name: '减肥'
      },
      {
        id: 4,
        imgUrl: '../../images/item5.jpg',
        name: '母婴'
      },
      {
        id: 5,
        imgUrl: '../../images/item6.jpg',
        name: '气节'
      },
      {
        id: 6,
        imgUrl: '../../images/item7.jpg',
        name: '常见食疗'
      }, {
        id: 7,
        imgUrl: '../../images/item8.jpg',
        name: '维生素'
      }
    ]
  },

  onLoad: function (options) {
    // var cityname=wx.getStorageSync('cityName');
    // console.log('onload第一次进入页面',cityname);
    // console.log(options);
    // if(options.cityName){//存在选中的城市
    //   this.setData({
    //     location: options.cityName
    //   })
    // }
    console.log(app);

    //1.进入页面获取信息列表---------------------
    wx.request({
      url: 'http://iwenwiki.com:3002/api/foods/list',
      data:{
        city:this.data.location,
        page: 1
      },
      success:res=>{
        console.log(res.data);
        if(res.data.status==200){
          this.setData({
            listArr: res.data.data.result
          })
        }else{
          console.log('请求无数据')
        }
      }
    })
  },


  onShow: function () {
    // console.log('onshow',app);
    // //请求数据-------------
    // console.log(app.globalData.cityName)
    //---------
    //获取本地存储----------------------------------------
    var cityname=wx.getStorageSync('cityName');
    console.log('onshow切换页面',cityname);
    if(cityname){
      this.setData({
        location:cityname
      })

      wx.request({
        url: 'http://iwenwiki.com:3002/api/foods/list',
        data:{
          city:cityname,
          page: 1
        },
        success:res=>{
          console.log(res.data);
          if(res.data.status==200){
            this.setData({
              listArr: res.data.data.result
            })
          }else{
            console.log('请求无数据');
            //清空listArr
            this.setData({
              listArr: []
            })
          }
        }
      })
    }
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
    console.log('下拉刷新页面数据');
    //修改num =1 
    //1.配置当前的页面json文件允许我们下拉页面 
    //2.下拉重新发送请求--请求接口里面最新数据 
    //3.获取新的数据后 替换之前的数据 
    // wx.request({
    //   url: 'http://iwenwiki.com:3002/api/foods/list',
    //   data:{
    //     city:'上海',
    //     page:1
    //   },
    //   success:res=>{
    //     console.log('更新的上海数据', res.data.data.result);
    //     this.setData({
    //       listArr: res.data.data.result
    //     })
    //   }
    // })

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {
    console.log('下拉到页面底部，加载更多数据');
    //当前的数据 页码++ 
    this.data.num++;
    console.log('请求下一个数据', this.data.num)

    //页面滚动到底部的时候 加载更多数据-----------------------
    //1.变量控制当前的第几次下拉--2.请求对应的页码接口---3.数据：老数据+新数据
    wx.request({
      url: 'http://iwenwiki.com:3002/api/foods/list',
      data: {
        city:this.data.location,
        page: this.data.num
      },
      success:res=>{
        console.log('加载更多的新数据', res.data)
        // console.log('加载更多的新数据', res.data.data.result);
        if(res.data.status==200){
            this.setData({
              listArr: this.data.listArr.concat(res.data.data.result)
            })
        }else{
          //没有数据了 
          this.setData({
            msg:'我是有底线的，没有更多数据'
          })
        }
      }

    })
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})