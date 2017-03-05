using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models.DTOs
{
	public class UserTriviaHistoryDTO
	{
		public int TriviaId { get; set; }
		public string TriviaText { get; set; }

		//-1 Downvote, 0 Default, 1 Upvote
		public int UserAction { get; set; }
	}
}  