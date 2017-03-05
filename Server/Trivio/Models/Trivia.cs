using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	public class Trivia
	{
		public int Id { get; set; }

		[Required]
		[StringLength(500)]
		[Index(IsUnique = true)]
		public string Text { get; set; }
		
		public int Upvotes { get; set; }
		public int Downvotes { get; set; }

		public virtual ICollection<UserTriviaHistory> UserHistory { get; set; }
	}
}