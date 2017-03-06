using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models.DTOs
{
	public class VoteDTO
	{
		public int UserVote { get; set; }
		public int UserId { get; set; }
		public int TriviaId { get; set; }
	}
}