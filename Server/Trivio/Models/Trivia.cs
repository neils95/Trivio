using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	public class Trivia
	{
		public int Id { get; set; }
		public string Text { get; set; }
		public int Upvotes { get; set; }
		public int Downvotes { get; set; }

		//Foreign key
		public Category Category { get; set; }
		public byte CategoryId { get; set; }

		//Foreign key
		public ICollection<TriviaHistoryModel> UserHistory { get; set; } = new List<TriviaHistoryModel>();

	}
}