const Guilded = require('guildscript');
const client = new Guilded.Client();
const fetch = require('node-fetch');

client.on('ready', () => {
	console.log(`Logged in as ${client.user.name}.`);
});

client.on('message', message => {
	let content = message.content.toString().trim();
	if(!message.channel.team && message.author != client.user && message.channel.constructor.name == "TextChannel")
	{
		if(content.substr(0, 15)=="Stand-Activate-"&&content.length==(15+31))
		{
			message.channel.send("Validating your activation key...");
			fetch("https://api.stand.one/internal_get_privilege/?0="+content.substr(15))
				.then(data=>data.text())
				.then(data=>
				{
					if(data=="0")
					{
						message.channel.send("Invalid activation key.");
					}
					else
					{
						let edition_names = {
							1: "Basic",
							2: "Regular",
							3: "Ultimate"
						};
						message.channel.send("Alright, that's a valid **"+edition_names[data]+" Edition** activation key, and you should have the role for that now. :)");
						let edition_role_ids = {
							1: "24499397",
							2: "24499396",
							3: "24499395",
						};
						client.request({
							method: "PUT",
							path: "teams/WlorOWQR/roles/"+edition_role_ids[data]+"/users/"+message.author.id,
						});
					}
				});
		}
		else
		{
			message.channel.send("Hi, I'm an automated service for getting the [Stand User] role in the Stand Community.\nTo do so, I simply require you to send me your activation key.");
		}
	}
});

client.login('guilded.bot@calamity.gg', 'ezyxjhc%!yHZAivCYhi7#4vc9ZGrme@odn3Ny!Tno6uLfKKGo&!$CaU3uyyc3U7&jXk5#tjc');
