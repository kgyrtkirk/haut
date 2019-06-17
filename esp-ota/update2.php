<?PHP

if ( getenv('WORKSPACE') == null ) {
	header($_SERVER["SERVER_PROTOCOL"].' 500 bajvan', true, 500);
	error_log("WORKSPACE unset");
	exit(1);
}

$workspace = getenv('WORKSPACE');
error_log("workspace: ".$workspace );


header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
    if(!isset($_SERVER[$name])) {
    	error_log("header problems: ".$name);
    	return false;
    }
    if($value && $_SERVER[$name] != $value) {
    	error_log("header value problems: ".$name);
    	return false;
    }
    return true;
}


function sendFile($path) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    error_log("UA!=ESP8266-http-Update");
    echo "only for ESP8266 updater!\n";
    exit();
}

error_log(print_R($_SERVER,TRUE));
if(
    !check_header('HTTP_X_ESP8266_STA_MAC') ||
    !check_header('HTTP_X_ESP8266_AP_MAC') ||
    !check_header('HTTP_X_ESP8266_FREE_SPACE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_MD5') ||
    !check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
    !check_header('HTTP_X_ESP8266_SDK_VERSION')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    error_log("needed headers");
    echo "only for ESP8266 updater! (header)\n";
    exit();
}
error_log("onlyaa");

$db = array (
		"60:01:94:0F:8A:5E" => "fx1",
		"60:01:94:0F:A8:5E" => "fx1",
		"X60:01:94:10:16:AE" => "esp_mqtt",	#	assembled pcb.1
		"60:01:94:0F:CE:44" => "esp_mqtt",	#	assembled pcb.2
);

if(!isset($db[$_SERVER['HTTP_X_ESP8266_STA_MAC']])) {
    error_log("unknown mac:" . $_SERVER['HTTP_X_ESP8266_STA_MAC']);
    header($_SERVER["SERVER_PROTOCOL"].' 500 ESP MAC not configured for updates', true, 500);
exit();
}

$fwName = $db[$_SERVER['HTTP_X_ESP8266_STA_MAC']];

$localBinary = sprintf("%s/%s/Release/%s.bin",$workspace,$fwName,$fwName);
error_log("serving binary from path:");
error_log("$localBinary");

// Check if version has been set and does not match, if not, check if
// MD5 hash between local binary and ESP8266 binary do not match if not.
// then no update has been found.
if((!check_header('HTTP_X_ESP8266_SDK_VERSION') && $db[$_SERVER['HTTP_X_ESP8266_STA_MAC']] != $_SERVER['HTTP_X_ESP8266_VERSION'])
    || $_SERVER["HTTP_X_ESP8266_SKETCH_MD5"] != md5_file($localBinary)) {
    sendFile($localBinary);
} else {
    header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
}

//header($_SERVER["SERVER_PROTOCOL"].' 500 no version for ESP MAC', true, 500);

?>
