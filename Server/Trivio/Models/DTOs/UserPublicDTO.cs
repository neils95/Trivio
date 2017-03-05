using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models.DTOs
{
	public class UserPublicDTO
	{
		public int Id { get; set; }
		public string Name { get; set; }
		public string Email { get; set; }
		public int TriviaCount { get; set; }
	}
}