using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TransferDatabase1.NewDb;
using TransferDatabase1.OldDb;

namespace TransferDatabase1
{
	class Program
	{
		static void Main(string[] args)
		{
			var oldDb = new OldDbContext();
			var newDb = new NewDbContext();
			try
			{
				foreach (var trivia in oldDb.Trivias)
				{
					newDb.Trivias.Add(new NewDb.Trivia()
					{
						Text = trivia.Text,
						Upvotes = 0,
						Downvotes = 0
					});
					Console.WriteLine(trivia.Id + ": " + trivia.Text);
					newDb.SaveChanges();
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
			}
			
		}
	}
}
