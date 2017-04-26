using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models.DTOs
{
	public class UserTriviaHistoryDTO
	{
		public Trivia Trivia { get; set; }

		//-1 Downvote, 0 Default, 1 Upvote
		public int UserVote { get; set; }
	}
}  