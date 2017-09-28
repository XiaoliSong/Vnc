function get_query_string(name){
     var reg = new RegExp("(^|&)"+ name +"=([^&]*)(&|$)");
     var r = window.location.search.substr(1).match(reg);
     if(r!=null) return  unescape(r[2]); 
	 return null;
}


document.getElementById("back_to_self").href="./file_person.html?id="+get_query_string("id")+"&pw="+get_query_string("pw")+"&to_see_id="+get_query_string("id")+"&page=0";


var page=get_query_string("page");
if(page==null||page<0){
	page=0;
}

//显示文件
function file_display(responseText){
	if(responseText==-1){
		var obj=document.getElementById("noti");
		obj.innerHTML="连数据库失败！"
		return;
	}
	else{
		if(responseText==-2) {
			var obj=document.getElementById("noti");
			obj.innerHTML="账号密码不匹配！"
			return;
		}
		else{
			if(responseText==""){
				var obj=document.getElementById("noti");
				obj.innerHTML="当前没有文件！";
				return;
			}
		}
	}
	
	var file=JSON.parse(responseText);

	var parent_obj=document.getElementById("file_table");
	for(var i=0;i<file.length;i++)
	{
		var d = new Date(file[i]['time']);
		var date=d.getFullYear()+"-"+(d.getMonth()+1)+"-"+d.getDate();
		var tr_ele=document.createElement("tr");//js新建元素
		tr_ele.innerHTML="<td>"+date+"</td>";
		var upload_id_info="<td><a href='file_person.html?id="+get_query_string("id")+"&pw="+get_query_string("pw")+"&to_see_id="+file[i]['upload_id']+"&page=0'>"+file[i]['upload_id']+"</a></td>";
		tr_ele.innerHTML+=upload_id_info;
		tr_ele.innerHTML+="<td><a href='./file/"+file[i]['upload_id']+"/"+file[i]['name']+"'>"+file[i]['name']+"</a></td>";
		parent_obj.appendChild(tr_ele);
	}

	if(file.length==0){
		var obj=document.getElementById("noti");
		obj.innerHTML="当前没有文件！";
	}
	if(file.length>=10){
		document.getElementById("next-page").style.visibility="visible";
	}
}


//ajax获取文件
function getFile(){
	var req_url="./file_public.php?id="+get_query_string("id")+"&pw="+get_query_string("pw")+"&page="+page;
	var xmlhttp;
	if (window.XMLHttpRequest){// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	
	else{// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}	

	xmlhttp.onreadystatechange=function() {
		if (xmlhttp.readyState==4 && xmlhttp.status==200){
			file_display(xmlhttp.responseText);
		}
	};
	xmlhttp.open("POST",req_url,true);
	xmlhttp.send();
}

getFile();

if(page==0){
	document.getElementById("former-page").style.visibility="hidden";
	document.getElementById("former-page").href="./file_public.html?id="+get_query_string("id")+"&pw="+get_query_string("pw")+"&page="+page;
}
else{
	document.getElementById("former-page").style.visibility="visible";
	var temp_page=page-1;
	document.getElementById("former-page").href="./file_public.html?id="+get_query_string("id")+"&pw="+get_query_string("pw")+"&page="+temp_page;
}

var temp_page=parseInt(page)+1;
document.getElementById("next-page").href="./file_public.html?id="+get_query_string("id")+"&pw="+get_query_string("pw")+"&page="+temp_page;
document.getElementById("page_noti").innerHTML=page;