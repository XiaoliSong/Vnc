<?php
	include "./connect.php";
	
	$defined_reg_pw="rootadmin";
	
	function del_file($dirName){
		if(file_exists($dirName) && $handle=opendir($dirName)){
			while(false!==($item = readdir($handle))){
				if($item!= "." && $item != ".."){
					if(file_exists($dirName.'/'.$item) && is_dir($dirName.'/'.$item)){
						del_file($dirName.'/'.$item);
					}else{
						if(unlink($dirName.'/'.$item)){
							return true;
						}
					}
				}
			}
			closedir( $handle);
		}
	}
	
	$os_name=PHP_OS;
	if(strpos($os_name,"Linux")!==false){
		$os_str="Linux";
	}else if(strpos($os_name,"WIN")!==false){
		$os_str="Windows";
	}

	$id=$_GET["id"];
	$pw=$_GET["pw"];
	$reg_pw=$_GET["reg_pw"];
	
	if($id==""){
		echo -5;
		return;
	}
	
	if($reg_pw!=$defined_reg_pw){
		echo 2;
	}
	else{
		$conn=connect();	
		if($conn==NULL){
			echo -2;
		}
		else{
			$sql="SELECT id,reg_time,intro from members WHERE id='$id'";
			$result=$conn->query($sql);
			$rowcount=$result->num_rows;
			if($rowcount==1){
				echo 0;
			}
			else{
				date_default_timezone_set('PRC'); 
				$time=date('Y-m-d H:i:s',time());
				$sql="INSERT INTO members SET id='$id',pw='$pw',reg_time='$time' ";//����
				if($result=$conn->query($sql)){
					
					if($os_str=="Linux"){
						if (!file_exists("./file_manager/file/".$add_id)){
							if(mkdir("./file_manager/file/".$add_id)){
								echo 1;
							}
							else{
								echo -4;
							}
						}
						else{
							del_file("./file_manager/file/".$id);
							echo 1;
						}
					}
					
					else{
						if (!file_exists(iconv("UTF-8","gb2312","./file_manager/file/".$id))){
							if(mkdir(iconv("UTF-8","gb2312","./file_manager/file/".$id))){
								echo 1;
							}
							else{
								echo -4;
							}
						}
						else{
							del_file(iconv("UTF-8","gb2312","./file_manager/file/".$id));
							echo 1;
						}
					}
				}
				else{
					echo -3;
				}
			}
		}
	}
?>