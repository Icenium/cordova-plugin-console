using System;
using System.Diagnostics;
using System.Linq;

namespace WPCordovaClassLib.Cordova.Commands
{
	public class AppBuilderOutput : BaseCommand
	{
#if DEBUG
		private readonly SocketLogger logger;

		public AppBuilderOutput()
		{
			this.logger = new SocketLogger();
			this.logger.BindPort();
		}
#endif

		public void logLevel(string options)
		{
			var message = ParseMessage(options);

			Debug.WriteLine(message);

#if DEBUG
			this.logger.Log(message);
#endif
		}
		
		private static string ParseMessage(string options)
		{
			var args = JSON.JsonHelper.Deserialize<string[]>(options);
			var level = args[0];
			var message = args[1];

			return "LOG".Equals(level, StringComparison.OrdinalIgnoreCase) ? message : string.Format("{0}: {1}", level, message);
		}
	}
}