using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Parser
{
	class Program
	{
		static int ErrorCount = 0;
		static void Main(string[] args)
		{
			for (int i = 1; i < 1000; i++)
			{
				//string url = "http://www.factsgenerator.com/";
				string url = "http://mentalfloss.com/amazingfactgenerator";
				foreach(string trivia in ExtractTrivia(url)) {
					Console.WriteLine(trivia);
					//AddTriviaToDb(trivia);
				}
			
				Console.WriteLine("i: " + i + "; Errors: " + ErrorCount);				
			}
		}
		static IEnumerable<string> ExtractTrivia(string url)
		{
			HtmlWeb web = new HtmlWeb();
			HtmlDocument doc = web.Load(url);

			//var triviaArray = doc.DocumentNode
			//					.SelectNodes("//span[@class='facttext']")
			//					.Select(p => p.InnerText.Replace("...", "")
			//						.Replace("\t", "").Trim());

			var triviaArray = doc.DocumentNode
								.SelectNodes("//div//p");
								
								//.SelectNodes("//p")
								//.Select(p => p.InnerText.Replace("...", "")
								//	.Replace("\t", "").Trim());


			return null;
		}

		static void AddTriviaToDb(string triviaText)
		{
			var Db = new TrivioTestDb();
			try
			{
				Db.Trivias.Add(new Trivia
				{
					Text = triviaText.Trim(),
					Upvotes = 0,
					Downvotes = 0
				});
				Db.SaveChanges();
			}
			catch(Exception e)
			{
				ErrorCount++;
				Console.WriteLine(e.Message);
			}

		}
	}

}
