// pages/myCart/myCart.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    // active:false,//默认是不选中  不出现删除
    // isSeleted:false,//当前的这个选项是否选中---默认不选中
    isSeletedAll:false,//全选状态
    startX:'',//开始的位置坐标点x轴
    list:[],//购物车列表信息
    num:0,//选中的个数   num>0 控制结算的状态的颜色
    totalPrice:'0.00',//总价格
    isShow:false,//控制结算的状态的颜色
  },
  //点击出现删除
  demo:function(){
    this.setData({
      active:!this.data.active
    })
  },
  //第一步：滑动出现删除-========================================================
  touchStart:function(e){
    // console.log(e);
    // console.log('startX开始位置',e.touches[0].clientX);

    this.setData({
      startX:e.touches[0].clientX
    })

  },
  //移动事件-==================================================================
  touchMove:function(e){
    // console.log('move',e.touches[0])
    //每个元素list都有一个标识0-----滑动谁 获取是哪个list里面的item 
    var index=e.currentTarget.dataset.index;
    console.log('滑动是第几个元素',index);
    var list=this.data.list;//获取购物车列表容器
    //目的：进入移动后  把其他的所有的属性active=false 只是当前的true 
    // for(var i=0;i<list.length;i++){
    //   list[i].active=false;
    // }
    
    var startX=this.data.startX;
    var moveX=e.touches[0].clientX;

    if(moveX < startX){//左滑
        list[index].active=true;//给当前的元素赋值属性true
    }else{
        list[index].active=false;
    }
    //修改当前的list后---同步更新数据data里面的变量list
    this.setData({
      list:list
    })
  },

  onLoad: function (options) {
      //获取数据
      // this.getShop();//只执行一次 
  },
  //第二步：获取购物车数据====================================================
  getShop:function(){
      var isSeletedAll=this.data.isSeletedAll;
      wx.request({
        url: 'http://iwenwiki.com:3002/api/cart/list',
        success:res=>{
          console.log(res.data);
          if(res.data.status==200){
              console.log(res.data.data.result);

              // this.setData({
              //   list:res.data.data.result
              // })

              //进入页面 判断全选是否选中：选中--所有的list列表都选中  否则不选中
              if(isSeletedAll){
                var list=res.data.data.result;
                for(var i=0;i<list.length;i++){
                    list[i].isSeleted=true;
                }
                
                this.setData({
                  list:list
                })

              }else{
                this.setData({
                    list:res.data.data.result
                  })
              }

          }else{
            console.log('购物车没有数据')
          }
        }
      })
  },
  //第三步：点击选中元素---====================================================
  //3.1 点击选中  再点击取消  3.2 点击操作的当前的元素   3.3 统计个数全选   3.4 控制结算高亮--至少有一个选中
  selectedList:function(e){
      //获取操作的当前元素index  获取操作list 
      var index=e.currentTarget.dataset.index;
      var list=this.data.list;
      var num=this.data.num;//个数

      //切换当前的元素的选中状态  点击取反之前的内容
      list[index].isSeleted=!list[index].isSeleted;

      //全选----：1.当前所有的都选中就是全选 num++  num==list.lenght  2.有一个不选中就是 不是全选   控制变量num数量
      if(list[index].isSeleted){//选中
          num++;
      }else{
        num--;
      }
      console.log('目前选中的个数',num);
      //全选--num++  num的总长度等于所有的list长度的时候
      if(num==list.length){ 
          this.setData({
            isSeletedAll:true
          })
      }else{
          this.setData({
            isSeletedAll:false
          })
      }
   
      // 判断当前的选中
      // if(num>0){
      //   this.setData({
      //     isShow:true
      //   })
      // }else{
      //   this.setData({
      //     isShow:false
      //   })
      // }


      this.setData({
        list:list,
        num:num
      })
      //获取价格的方法===========
      this.gototalPrice();

    },

  //第四步：计算价格--函数=====================================================
  //4.1 获取data的list容器  4.2 遍历查看list里面选中的元素   计算选中的元素  4.3 计算=当前的元素的数量*价格
  gototalPrice:function(){
    var list=this.data.list;
    var sum=0;//总价格
    //遍历list容器 获取选中的内容---
    for(var i=0;i<list.length;i++){
      //list[i].isSeleted 是否选中 true选中
      if(list[i].isSeleted){
        sum+=list[i].price*list[i].num;
      }
    }
    //更改价格 
    this.setData({
      totalPrice:sum.toFixed(2)
    })

  },

  //第五步:点击全选---========================================================
  //5.1 全选按钮加事件-点击的时候=--对状态取反 5.2 选中：所有的list每一项都选中  5.3 取消全选--list取消选中  5.4价格
  seletedAll:function(){
    //获取全选按钮之前的状态取反
    var isSeletedAll=!this.data.isSeletedAll;
    //获取list
    var list=this.data.list;
    var num=this.data.num;

    //控制所有的list每一项是否选中
    for(var i=0;i<list.length;i++){
        list[i].isSeleted=isSeletedAll;
    }

    if(isSeletedAll){//全选
        num=list.length;
    }else{
      num=0;
    }
    //更新数据
    this.setData({
      list:list,
      isSeletedAll:isSeletedAll,
      num:num
    })

    //价格
    this.gototalPrice();

  },

  //第六个:点击++ 修改购物车商品数量=============================================
  add:function(e){
      //1.获取当前的操作的元素   2.获取列表容器list 
      var list=this.data.list;
      var index=e.currentTarget.dataset.index;
      // console.log(index);
      //获取当前的元素的目前的数量
      var num=list[index].num;

      //2.点击当前的数量 num++   提交后台接口
      num++;
      console.log('增加的属性',num);
      list[index].num=num;
      //3.请求接口
      wx.request({
        url: 'http://iwenwiki.com:3002/api/cart/update',
        data:{
          id:list[index].id,
          num:num
        },
        success:res=>{
          console.log(res.data);
          if(res.data.status==200){//修改 数量
            this.setData({
              list:list
            })
          }
        }
      })

      // 获取总的价格
      this.gototalPrice()
  },
  //第七步：点击 --   修改购物车数量============================================
  reduce:function(e){
    //1.获取当前的操作的元素   2.获取列表容器list 
    var list=this.data.list;
    var index=e.currentTarget.dataset.index;

    //获取当前的元素的目前的数量
    var num=list[index].num;
    num--;
    if(num<1){
      wx.showToast({
        title: '数量最少为1',
        icon:'none'
      })
      return;
    }
    //修改
    console.log('减少后的属性',num);
      list[index].num=num;
      //3.请求接口
      wx.request({
        url: 'http://iwenwiki.com:3002/api/cart/update',
        data:{
          id:list[index].id,
          num:num
        },
        success:res=>{
          console.log(res.data);
          if(res.data.status==200){//修改 数量
            this.setData({
              list:list
            })
          }
        }
      })
      // 获取总的价格
      this.gototalPrice()

  },

  //第八步：删除 ==============================================================
  //8.1 参数-获取删除的是哪个元素   8.2 执行删除接口   8.3 删除后页面数据不存在
  delete:function(e){
      var index=e.currentTarget.dataset.index;
      var list=this.data.list;

      //执行删除---接口
      wx.request({
        url: 'http://iwenwiki.com:3002/api/cart/delete',
        data:{
          id:list[index].id
        },
        success:res=>{
          console.log(res.data);
          if(res.data.status==200){
            wx.showToast({
              title: '删除成功',
              icon:'none'
            })
            //界面：数据消失-----
            //直接删除本地的数据data里面的list容器 对应的这个数据 
            list.splice(index,1);//删除 splice(index下标,1删除个数)
            //修改list 
            this.setData({
              list:list
            })

            // //获取价格：
            this.gototalPrice();
          }
        }
      })

  },

  //9.结算==========================================
  jiesuan(){
    //1.先判断是否授权登录  登录过有openid /user   2.直接接口--购买
    // var user=wx.getStorageSync('openid');//之前登录的时候存储的字段
    // if(user){
    //   //直接购买 
    //   //跳转完成界面

    // }else{
    //   //弹框提示 --先授权--再登录

    //   // //跳转到授权界面 
    //   // wx.switchTab({
    //   //   url: '/pages/about/about',
    //   // })
    // }

    //==============================
    //9.1 直接购买   9.2 跳转页面
    wx.request({
      method:'post',
      url: 'http://iwenwiki.com:3002/api/buy',
      data:{
        user:'qq',
        id:'123'
      },
      header: {
        'content-type': 'application/x-www-form-urlencoded' // post默认值
      },
      success:res=>{
        console.log(res.data);
        //页面跳转 关闭当前页面，跳转到应用内的某个页面
      if(res.data.status==200){
          wx.redirectTo({
          url: '/pages/complete/complete?mark='+res.data.msg,
        })
      }
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
    this.getShop();//进入页面===重新获取最新购物车信息 

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