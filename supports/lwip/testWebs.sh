
SVR_HOST=192.168.166.2
SVR_PORT=80

PAGES=(
		"" 
		"logo.jpg"
		"info" 
		"media"
		"video.sdp"
		"audio.sdp"
)


REST_APIS=(
		"service" 
)

# using string array

echo "###################################"
echo "Test Pages...."
echo "###################################"
for page in "${PAGES[@]}"
do
	let i=i+1
	echo "################$i Request: "
	echo "            GET http://$SVR_HOST:$SVR_PORT/$page"
	curl -H "Content-Type: text/html" -X GET http://$SVR_HOST:$SVR_PORT/$page
	echo ""
	echo ""
	done



echo ""
echo ""
echo "###################################"
echo "Test REST API...."
echo "###################################"
for page in "${REST_APIS[@]}"
do
	let i=i+1
	echo "###############$i Request: "
	echo "             GET http://$SVR_HOST:$SVR_PORT/$page"
	curl -H "Content-Type: application/json" -X GET http://$SVR_HOST:$SVR_PORT/$page
	echo ""
	echo ""
	done
