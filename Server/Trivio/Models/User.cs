using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	public partial class User
	{
		public int Id { get; set; }
		public string FirstName { get; set; }
		public string LastName { get; set; }
		public string email { get; set; }
		public string password { get; set; }

		//Unique identifier for User
		public int Token { get; set; }
		
		//Ids of trivia history for user
		public ICollection<Trivia> TrivaHistory { get; set; }

		//Number of facts user has already downloaded
		public int TriviaCount { get; set; }

		public User()
		{
			TrivaHistory = new List<Trivia>();
		}
	}
}