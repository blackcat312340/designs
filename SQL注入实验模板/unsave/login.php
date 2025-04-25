<html>
<head>
<style>
body {
display: flex;
flex-direction: column;
align-items: center;
justify-content: center;
min-height: 60vh;
font-size:20px;
margin: 0;
}
text {
font-size:60px;
}
table, th, td {
  border: 1px solid black;
  border-collapse: collapse;
  font-size:20px;
}
</style>
<title>login</title>
</head>
<body>
    <?php
    //这里改成你自己的sqlserver服务器名称
        $serverName = "";  
        //乱码的话，charset改成UTF-8则php输出正常
        $connectionInfo = array( "Database"=>"payload","CharacterSet" => "UTF-8","Encrypt"=>"false","trustServerCertificate"=>"true");  

        $conn = sqlsrv_connect( $serverName, $connectionInfo);  
        if( $conn === false )  
        {  
            echo "<text>"."连接数据库失败"."</text>"."</br>";  
            die( print_r( sqlsrv_errors(), true));  
        }
        $user=$_POST['name'];
        $psd=$_POST['passwd'];
        $tsql = "SELECT username,passwd from info where username='$user' and passwd='$psd'";
        echo $tsql;  
        $stmt = sqlsrv_query( $conn, $tsql);  
        if( $stmt === false )  
        {  
            echo "<text>"."查询错误"."</text>"."</br>";  
            die( print_r( sqlsrv_errors(), true));  
        }
        $result="<text>"."成功登录"."</text>"."<br/>".
        "<table>
            <tr>
                <th>用户</th>
                <th>密码</th>
            </tr>";
        $hasdata=false;
        while ($row = sqlsrv_fetch_array($stmt, SQLSRV_FETCH_ASSOC)) {
            $hasdata=true;
            $result.= "<tr>"."<th>" . $row['username'] . "</th>";
            $result.= "<th>". $row['passwd'] . "</tr>";
        }
        if($hasdata===false) {
            die( print_r( "<text>"."没有这个用户"."</text>"."</br>" ,true));
        }
        $result.="</table>";
        echo $result;
        sqlsrv_free_stmt( $stmt);  
        sqlsrv_close( $conn);  
    ?>
</body>
</html>