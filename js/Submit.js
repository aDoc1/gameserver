var apiKEY = "05fd965e0acac19350a76e945742d4d026c4a984";

$(document).ready(function(){
	$('.search').val('');
	$('.searchError').hide();
	$('.searchResults').hide();
	$('.Description').hide();
	$('.Publishers').hide(); 
	$('.Developers').hide(); 
	$('.Platforms').hide(); 
	$('.Genres').hide(); 
	$('.ReleaseDate').hide(); 
	$('.BoxArt').hide();
	$('.resultsContainer').hide();
	

	$('.submit').click(function(){
		sendRequest();
	});
	$('.search').keyup(function(event){    
		if(event.keyCode == 13){
			$('.submit').trigger('click');
		}
	});
});

function sendRequest(){
	var gameURL;
	
	var search = $.trim($('.search').val());
	$('.searchResults').hide();
    $('.resultsContainer').hide();
	
	$.ajax({
		url: 'http://www.giantbomb.com/api/search/?query=' + search + '&field_list=api_detail_url,name,id&resources=game&api_key=' + apiKEY + '&format=jsonp&json_callback=myCallback',
		dataType: 'jsonp',
		jsonpCallback: 'myCallback',
		success: (function myCallback(response){
			$('.searchError').hide();
			$('.searchResults').hide();
			$('.gameRes').scrollTop(0);
			
			if(response.results.length == 1 && response.results[0].name.toLowerCase() == search.toLowerCase()) 
			{
				gameURL = response.results[0].api_detail_url;
				getGameInfo(gameURL);
			}
			else if (response.results.length >= 1) {
				$('.searchResults').show();
				var gameList = $(".gameRes");
				gameList.empty();
				$.each(response.results, function (i, object) {
                    if (i == 0)
                        gameList.append($('<option selected="selected"></option>').val(object.api_detail_url).html(object.name));
                    else
                        gameList.append($('<option></option>').val(object.api_detail_url).html(object.name));
                })
                if (response.results.length > 10) {
                    $('.gameRes').attr('size', 10);
                }
                else {
                    $('.gameRes').attr('size', response.results.length);
                }
               /* $('option').hover(function(){
					$this.val();
                });
                console.log($('.gameRes').find('option:selected'));*/
			} else {
				$('.searchError').show();
				$('.searchError').html("<p>Game could not be found, make sure it is spelled correctly.</p>");
			}
		})
	}).fail(function () {
        $('.searchError').show();
        $('.searchError').html("<p>Problem with the gamebomb site, could not retrieve game information.</p>");
    });
}

function getGameInfo(gameURL)
{
	var publishers = new Array();
	var developers = new Array();
	var platforms = new Array();
	var genres = new Array();
	var releaseDate = "";
	var boxArt = "";
	var description = "";
	var optChecked;
	
	$.ajax({
		url: gameURL +'?api_key=' + apiKEY + '&field_list=publishers,developers,platforms,genres,image,original_release_date,deck&format=jsonp&json_callback=mySecondCallback',
		dataType: 'jsonp',
		jsonpCallback: 'mySecondCallback',
		success: (function mySecondCallback(data){
			var result = '';
			
			if(data.results.publishers != null){
				$.each(data.results.publishers, function(j, obj) {
					publishers[j] = obj.name;
				});
			}
			if(data.results.developers != null) {
				$.each(data.results.developers, function(j, obj) {
					developers[j] = obj.name;
				});
			}
			if(data.results.genres != null){
				$.each(data.results.genres, function(j, obj) {
					genres[j] = obj.name;
				});
			}
			if(data.results.platforms != null){
				$.each(data.results.platforms, function(j, obj) {
					platforms[j] = obj.name;
				});
			}
			
			if(data.results.platforms != null){
				boxArt = data.results.image.small_url;
			}
			if(data.results.platforms != null){
				releaseDate = data.results.original_release_date;
			}
			if(data.results.platforms != null){
				description = data.results.deck;
			}
			
			$('.Publishers').html(function() {
				result = '';
				$.each(publishers, function(j, obj){
					result = result + obj + "<br />";
				})
				return result;
			});
			
			$('.Developers').html(function() {
				result = '';
				$.each(developers, function(j, obj){
					result = result + obj + "<br />";
				})
				return result;
			});
			
			$('.Platforms').html(function() {
				result = '';
				$.each(platforms, function(j, obj){
					result = result + obj + "<br />";
				})
				return result;
			}); 
			
			$('.Genres').html(function() {
				result = '';
				$.each(genres, function(j, obj){
					result = result + obj + "<br />";
				})
				return result;
			}); 
			
			var relDate = Date.parse(releaseDate);
			relDate = relDate.toLocaleDateString();
			$('.ReleaseDate').html(relDate); 
			$('.BoxArt').html("<img src='" + boxArt +"' alt='' width='100%' height='100%'></img>"); 
			$('.Description').html(description);
			
			optChecked = $('input[name="group1"]:checked').val();
            if (optChecked < 1 || optChecked > 7)
                optChecked = 1;

            onRadioClick(parseInt(optChecked));

            $('.resultsContainer').show();
		})
	})
}

function onRadioClick(inx) 
{
	$('.Description').hide();
	$('.Publishers').hide(); 
	$('.Developers').hide(); 
	$('.Platforms').hide(); 
	$('.Genres').hide(); 
	$('.ReleaseDate').hide(); 
	$('.BoxArt').hide();
	
	switch (inx) {
		case 1: 
			$('.Description').show();  
			break;
		case 2: 
			$('.Publishers').show();  
			break;
		case 3:
			$('.Developers').show();  
			break;
		case 4:
			$('.Platforms').show();  
			break;
		case 5:
			$('.Genres').show();  
			break;
		case 6:
			$('.ReleaseDate').show();  
			break;
		case 7:
			$('.BoxArt').show();  
			break;
	}
}

function onDropdownClick()
{
	var selectedGame = $('.gameRes').find('option:selected');
	$('.search').val(selectedGame.text());
	$('.searchResults').hide();
	getGameInfo(selectedGame.val());
}
