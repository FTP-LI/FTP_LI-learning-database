/**
 * 网络请求：
 *  method: string 请求的方式 get/post
 *  params  发送的参数{ key:value } 
 *  url:    请求的接口地址
 *  message 信息弹框交互数据  loading 
 *  callback 成功的回调函数
 *  error  失败的回调函数
 * 
 */
function http(method,params,url,message,callback,error){
  //loading
  if(message){
    wx.showLoading({
      title: message,
    })
  }
  //网络请求
  wx.request({
    method:method,
    url: 'http://iwenwiki.com:3002'+url,
    data:params,
    header: {
      'content-type': 'application/x-www-form-urlencoded' // post默认值
    },
    success:res=>{
      console.log(res.data);
      if(res.data.status==200){
        wx.showToast({
          title: '数据加载完毕',
        })
        //返回数据
        callback(res.data);

      }else{
        error('没有数据')
      }
    },
    fail:err=>{
      error('请求失败');
    },
    complete:function(){
      wx.hideLoading();
    }
  })
}
//暴露了  
module.exports=http;

//语法：
// module.exports={ http:http }