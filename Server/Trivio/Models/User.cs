using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	public class User
	{
		public int Id { get; set; }
		public string Name { get; set; }
		
		[Required][StringLength(50)]
		[Index(IsUnique = true)]
		public string Email { get; set; }

		[Required]
		public string Password { get; set; }

		//Number of facts user has already downloaded
		public int TriviaCount { get; set; }

		//Enable trivia history
		//public ICollection<UserTriviaHistory> TriviaHistory { get; set; }
	}
}