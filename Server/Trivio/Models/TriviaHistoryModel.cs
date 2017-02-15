using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	//One entry in TriviaHistory. Used to track trivia History for user.
	public class TriviaHistoryModel
	{
		public int Id { get; set; }

		//Every TriviaHistory is associated with a single user:Foreign Key
		public User User { get; set; }
		public int UserId { get; set; }
		
		//Every TriviaHistory is associated with a single trivia:Foreign key
		public Trivia Trivia { get; set; }
		public int TriviaId { get; set; }

		//1, -1 ,0 for Upvote, Downvote, No Vote
		public int UserVote { get; set; }

	}
}