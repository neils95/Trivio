using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	public class UserTriviaHistory
	{
		//User information:Foreign Key
		public int UserId { get; set; }
		public User User { get; set; }
		
		//Trivia information: Foreign Key
		public int TriviaId { get; set; }
		public Trivia Trivia { get; set; }

		//-1 DOwnvote, 0 Default, 1 Upvote
		public int UserAction { get; set; }

	}
}